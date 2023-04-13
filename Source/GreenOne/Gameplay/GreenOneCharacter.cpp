// Copyright Epic Games, Inc. All Rights Reserved.

#include "GreenOneCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "GreenOne/Gameplay/Common/AttackMelee.h"
#include "GreenOne/Core/CustomCharacterMovement/CustomCharacterMovementComponent.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerBase.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerFactory.h"

//////////////////////////////////////////////////////////////////////////
// AGreenOneCharacter

TSubclassOf<UFertilizerBase> AGreenOneCharacter::GetCurrentEffect(FertilizerType Type)
{
	if (!Effects.Contains(Type))
	{
		return TSubclassOf<UFertilizerBase>();
	}
	return *Effects.Find(Type);
}

bool AGreenOneCharacter::IsCurrentEffectExist(FertilizerType Type)
{
	if(!Effects.Find(Type)) return false;
	return true;
}

AGreenOneCharacter::AGreenOneCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Add AttackMeleeComponent
	AttackMeleeComponent = CreateDefaultSubobject<UAttackMelee>(TEXT("AttackMelee"));
	if(!AttackMeleeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No AttackMeleeComponent Found"));
	}
	
	// Add TargetMuzzle
	TargetMuzzle = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleTarget"));
	TargetMuzzle->SetupAttachment(GetMesh());
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ShootCooldown = 1.f / 3.f;
	ShootBloom = 0.f;
	CanShoot = true;
}

#if WITH_EDITOR
void AGreenOneCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, Health))
	{
		MaxHealth = Health;
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, SocketMuzzle))
	{
		//TargetMuzzle->SetAttachSocketName(SocketMuzzle);
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// Input

void AGreenOneCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::Move);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, CustomCharacterMovementComponent, &UCustomCharacterMovementComponent::Dash);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::InputJump);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AGreenOneCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AGreenOneCharacter::StopShoot);
		EnhancedInputComponent->BindAction(AttackMeleeAction, ETriggerEvent::Triggered, AttackMeleeComponent, &UAttackMelee::Attack);
		EnhancedInputComponent->BindAction(EnableFertilizerAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::EnableFertilizer);
		EnhancedInputComponent->BindAction(ChangeFertilizerTypeAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::ChangeFertilizerType);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::Interact);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::TogglePauseGame);
	}
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

}

void AGreenOneCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CustomCharacterMovementComponent = Cast<UCustomCharacterMovementComponent>(Super::GetCharacterMovement());
}

void AGreenOneCharacter::PlayerDead()
{
	if (!bIsDead)
	{
		bIsDead = true;
		GetMesh()->SetSimulatePhysics(true);
		GetMovementComponent()->SetActive(false);
		OnPlayerDeath.Broadcast();
	}
}

void AGreenOneCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	MaxHealth = Health;
	ShootCooldownRemaining = ShootCooldown;
}

void AGreenOneCharacter::Tick(float DeltaSeconds)
{
	
	Super::Tick(DeltaSeconds);
	ShootTick(DeltaSeconds);
}

void AGreenOneCharacter::InputJump(const FInputActionValue& Value)
{
	bool bIsJumping = Value.Get<bool>();
	if (bIsJumping)
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void AGreenOneCharacter::EnableFertilizer()
{
	UE_LOG(LogTemp, Warning, TEXT("Fertilizer Enable"));
	
	if(EFertilizerType == FertilizerType::None)
		EFertilizerType = FertilizerType::SlowDown;
	else if(EFertilizerType != FertilizerType::None)
		EFertilizerType = FertilizerType::None;
}

void AGreenOneCharacter::ChangeFertilizerType()
{
	if(EFertilizerType == FertilizerType::AttackBonus)
	{
		EFertilizerType = FertilizerType::SlowDown;
		UE_LOG(LogTemp, Warning, TEXT("Fertilizer SlowDown"));
	}else if(EFertilizerType == FertilizerType::SlowDown)
	{
		EFertilizerType = FertilizerType::AttackBonus;
		UE_LOG(LogTemp, Warning, TEXT("Fertilizer AttackBonus"));
	}
	
}

void AGreenOneCharacter::Interact()
{
	// TODO
}

void AGreenOneCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGreenOneCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AGreenOneCharacter::IsAttacking()
{
	return IsAtk;
}

void AGreenOneCharacter::EntityTakeDamage_Implementation(float damage, FName BoneNameHit, AActor* DamageSource)
{
	if(Immortal) return;
	
	Health -= damage;
	if (Health <= 0)
	if(!Invisible) Health -= damage;
	if(Health <= 0)
	{
		PlayerDead();
		Health = 0.f;
	}
	OnTakeDamage.Broadcast();
}

float AGreenOneCharacter::GetHealthPercent()
{
	return Health / MaxHealth;
}

void AGreenOneCharacter::Shoot()
{
	TurnCamera();
	if (!CanShoot) { return; }

	CanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(ShootHandler, this, &AGreenOneCharacter::ShootRafale, ShootCooldown, true);
	ShootRafale();
}

void AGreenOneCharacter::StopShoot()
{
	IsRegenerate();
	if (ShootHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandler);
	}
}

void AGreenOneCharacter::ShootRafale()
{
	FHitResult OutHit;
	const FVector  StartLocation = TargetMuzzle->GetComponentLocation();

	if (!IsTouchSomething)
	{
		LocationToAim = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * ShootDistance);
	}

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, LocationToAim, ECC_Camera))
	{
		if(ShootParticule)
		{
			const FRotator ParticuleRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, LocationToAim);
			UNiagaraComponent* CurrentShootPart = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShootParticule, StartLocation, ParticuleRotation, FVector::One());
			CurrentShootPart->SetFloatParameter("ShootDistance", FVector::Distance(StartLocation, OutHit.Location));
			CurrentShootPart->SetColorParameter("ShootColor", FColor::White);
		}
		if (ImpactParticules)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactParticules, OutHit.Location, FRotator::ZeroRotator, FVector::One());
		}
		if (OutHit.GetActor() == nullptr)
		{
			return;
		}
		if (ABaseEnnemy* CurrentTargetHit = Cast<ABaseEnnemy>(OutHit.GetActor()))
		{
			if (CurrentTargetHit->Implements<UEntityGame>())
			{
				IEntityGame::Execute_EntityTakeDamage(CurrentTargetHit, DamagePlayer, OutHit.BoneName, this);
				if(IsCurrentEffectExist(EFertilizerType))
				{
					if(UFertilizerBase* Fertilizer = FertilizerFactory::Factory(EFertilizerType, GetCurrentEffect(EFertilizerType)))
					{
						IEntityGame::Execute_EntityTakeEffect(CurrentTargetHit, Fertilizer, this);	
					}	
				}
				OnHitEnnemy.Broadcast(CurrentTargetHit);
			}
		}
	}
	if (ShootParticule)
	{
		const FRotator ParticuleRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, LocationToAim);
		UNiagaraComponent* CurrentShootPart = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShootParticule, StartLocation, ParticuleRotation, FVector::One());
		if(CurrentShootPart)
		CurrentShootPart->SetFloatParameter("ShootDistance", ShootDistance);
	}
}

void AGreenOneCharacter::ShootTick(float deltatime)
{
	if (!CanShoot)
	{
		ShootCooldownRemaining -= deltatime;
		if (ShootCooldownRemaining <= 0.f)
		{
			ShootCooldownRemaining = ShootCooldown;
			CanShoot = true;
		}
	}
	else
	{
		APlayerCameraManager* CameraRef = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FHitResult OutHit;

		float DegreeRotation = UKismetMathLibrary::Lerp(0.f, 360.f, ShootBloom);
		FVector StartLocation = CameraRef->GetCameraLocation();
		FVector EndLocation = StartLocation + UKismetMathLibrary::RandomUnitVectorInConeInDegrees(CameraRef->GetActorForwardVector(), DegreeRotation) * ShootDistance;
		IsTouchSomething = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Camera);
		if (IsTouchSomething)
		{
			if (OutHit.GetActor() == this)
			{
				LocationToAim = TargetMuzzle->GetComponentLocation() + (GetActorForwardVector() * ShootDistance);
			}
			else
			{
				LocationToAim = (OutHit.Location - TargetMuzzle->GetComponentLocation()) * ShootDistance;
			}
		}
	}
}

//This function is used to toggle the pause state of the game. It first checks if a pause widget class has been set, and if not, it logs a warning. It then casts the
// controller to a player controller and checks if it is valid. If it is, it checks if the world is paused. If it is, it creates a pause widget and adds it to the view
//port, and sets the input mode to game and UI. If the world is not paused, it sets the visibility of the pause widget to collapsed and sets the input mode to game
// only.
void AGreenOneCharacter::TogglePauseGame()
{
	if (!PauseWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wesh t'as oublier de mettre un widget dans le pause."));
		return;
	}

	APlayerController* ControllerRef = Cast<APlayerController>(Controller);

	if (!ControllerRef)
		{ return; }

	if (GetWorld()->IsPaused())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		if (PauseWidgetRef)
		{
			PauseWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
			ControllerRef->SetShowMouseCursor(false);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(ControllerRef);
		}
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (!PauseWidgetRef)
		{
			PauseWidgetRef = CreateWidget<UUserWidget>(ControllerRef, PauseWidgetClass);
			PauseWidgetRef->AddToViewport();
		}
		else
		{
			PauseWidgetRef->SetVisibility(ESlateVisibility::Visible);
		}
		ControllerRef->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(ControllerRef, PauseWidgetRef);
	}
}

void AGreenOneCharacter::TurnCamera()
{
	SetActorRotation(FRotator(GetActorRotation().Roll, GetFollowCamera()->GetComponentRotation().Yaw, GetActorRotation().Pitch));
}

void AGreenOneCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 

		float MovementVectorY = 0.f;

		if(MovementVector.Y < 0.f) 
		{
			MovementVectorY = (MovementVector.Y * ReverseSlow);
		}
		else
		{
			MovementVectorY = MovementVector.Y;
		}

		float MovementVectorX = 0.f;

		MovementVectorX = (MovementVector.X * LateralSlow);

		AddMovementInput(ForwardDirection, MovementVectorY);
		AddMovementInput(RightDirection, MovementVectorX);
	}
}


void AGreenOneCharacter::IsRegenerate()
{
	if(IsCombatMode)
	{
		Health = Health + 10;
	}
	
}

