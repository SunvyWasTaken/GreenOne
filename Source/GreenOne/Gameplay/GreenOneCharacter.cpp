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
#include "Kismet/KismetSystemLibrary.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "GreenOne/Gameplay/Common/AttackMelee.h"
#include "GreenOne/Core/CustomCharacterMovement/CustomCharacterMovementComponent.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerBase.h"
#include "GreenOne/Core/Factory/Fertilizer/FertilizerFactory.h"
#include "Interactible/InteractibleActorInterface.h"
#include "Weapon/Fertilizer/FertilizerTankComponent.h"

//////////////////////////////////////////////////////////////////////////
// AGreenOneCharacter

AGreenOneCharacter::AGreenOneCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	InitializeCustomCharacterMovementComponent();
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	
	GetCustomCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCustomCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCustomCharacterMovement()->AirControl = 0.35f;
	GetCustomCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCustomCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCustomCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCustomCharacterMovement()->bRequestedMoveUseAcceleration = false;

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
	TargetMuzzle->SetupAttachment(GetMesh(), FName("hand_rSocket"));

	HealComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HealComp"));
	HealComponent->SetupAttachment(RootComponent);
	HealComponent->Activate(false);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ShootCooldown = 3.f;
	ShootBloom = 0.f;
	CanShoot = true;

	JumpMaxCount = 2;

	MaxHealth = Health;
	ShootCooldownRemaining = 1.f / ShootCooldown;

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundObject(TEXT("/Game/GreenOne/SFX/MainCharater/MS_Tire_graine"));
	if (SoundObject.Object != nullptr)
	{
		ShootSound = SoundObject.Object;
	}
	
}

void AGreenOneCharacter::InitializeCustomCharacterMovementComponent()
{
	CustomCharacterMovementComponent = Cast<UCustomCharacterMovementComponent>(Super::GetCharacterMovement());
}

#if WITH_EDITOR
void AGreenOneCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, Health))
	{
		MaxHealth = Health;
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, ShootCooldown))
	{
		ShootCooldownRemaining = 1.f / ShootCooldown;
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, Health))
	{
		MaxHealth = Health;
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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::Dash);
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

	FertilizerTankComponent = FindComponentByClass<UFertilizerTankComponent>();
	if(!FertilizerTankComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No FertilizerTankComponent Found"));
	}
}

void AGreenOneCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Respawn();
}

void AGreenOneCharacter::SetLastTouchLocation(FVector Location)
{
	LastTouchLocation = Location;
	return;
}

void AGreenOneCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ShootTick(DeltaSeconds);
	Regenerate(DeltaSeconds);
	
	// Reset Dash Vector Direction
	// FVector2D reset = FVector2D(0, 0);
	// GetCustomCharacterMovement()->SetDashDirectionVector(reset);
}

void AGreenOneCharacter::InputJump(const FInputActionValue& Value)
{	
	if (Value.Get<bool>())
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
	if(!FertilizerTankComponent) return;
	UE_LOG(LogTemp, Warning, TEXT("Fertilizer Enable"));

	FertilizerTankComponent->Equip();
}

void AGreenOneCharacter::ChangeFertilizerType()
{
	if(!FertilizerTankComponent) return;

	FertilizerTankComponent->SwitchFertilizerEquip();
}

void AGreenOneCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact"));
	
	if(!InteractibleActorInterface) return;

	InteractibleActorInterface->Action(this);
}

void AGreenOneCharacter::Respawn()
{
	SetActorLocation(LastTouchLocation);
	if (this->Implements<UEntityGame>())
	{
		IEntityGame::Execute_EntityTakeDamage(this, MaxHealth*0.1f, FName("None"), this);
	}
	GetCharacterMovement()->StopMovementImmediately();
}

void AGreenOneCharacter::SetInteractibleActor(IInteractibleActorInterface* InteractibleActor)
{
	if(InteractibleActor)
	{
		InteractibleActorInterface = InteractibleActor;		
	}else
	{
		InteractibleActorInterface = nullptr;
	}
}

IInteractibleActorInterface* AGreenOneCharacter::GetInteractibleActor() const
{
	return InteractibleActorInterface;
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
	UE_LOG(LogTemp, Warning, TEXT("loose life"));
	IsCombatMode = true;
	if (Health <= 0)
	if(!Invisible) Health -= damage;
	if(Health <= 0)
	{
		PlayerDead();
		Health = 0.f;
	}
	CanRegenerate();
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
	GetWorld()->GetTimerManager().SetTimer(ShootHandler, this, &AGreenOneCharacter::ShootRafale, 1/ShootCooldown, true);
	ShootRafale();
}

void AGreenOneCharacter::StopShoot()
{
	if (ShootHandler.IsValid())
	{
		CanRegenerate();
		GetWorld()->GetTimerManager().ClearTimer(ShootHandler);
	}
}

void AGreenOneCharacter::ShootRafale()
{
	//OnShootDelegate.Broadcast(EFertilizerType);
	if(FertilizerTankComponent)
	{
		FertilizerTankComponent->OnShoot();
	}
	
	FHitResult OutHit;
	const FVector StartLocation = TargetMuzzle->GetComponentLocation();
	FVector EndLocation = LocationToAim - StartLocation;
	EndLocation.Normalize();
	EndLocation = StartLocation + (EndLocation * ShootDistance);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Camera))
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
			//IsCombatMode = true;
			if (CurrentTargetHit->Implements<UEntityGame>())
			{
				IEntityGame::Execute_EntityTakeDamage(CurrentTargetHit, DamagePlayer, OutHit.BoneName, this);
				
				if(FertilizerTankComponent && !FertilizerTankComponent->IsTankEmpty(FertilizerTankComponent->GetCurrentFertilizerType()))
				{
					if(UFertilizerBase* Fertilizer = FertilizerTankComponent->GetEffect())
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
	UGameplayStatics::PlaySound2D(GetWorld(), ShootSound);
}

void AGreenOneCharacter::ShootTick(float deltatime)
{
	if (!CanShoot)
	{
		ShootCooldownRemaining -= deltatime;
		if (ShootCooldownRemaining <= 0.f)
		{
			ShootCooldownRemaining = 1/ShootCooldown;
			CanShoot = true;
		}
	}
	else
	{
		APlayerCameraManager* CameraRef = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FHitResult OutHit;

		//float DegreeRotation = UKismetMathLibrary::Lerp(0.f, 360.f, ShootBloom);
		FVector StartLocation = CameraRef->GetCameraLocation();
		FVector EndLocation = StartLocation + (CameraRef->GetActorForwardVector() * ShootDistance); //UKismetMathLibrary::RandomUnitVectorInConeInDegrees(CameraRef->GetActorForwardVector(), DegreeRotation) * ShootDistance;
		IsTouchSomething = GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Camera);
		if (IsTouchSomething)
		{
			if (OutHit.GetActor() == this)
			{
				LocationToAim = TargetMuzzle->GetComponentLocation() + (GetActorForwardVector() * (ShootDistance + CameraBoom->TargetArmLength));
			}
			else
			{
				LocationToAim = OutHit.Location;
			}
		}
		else
		{
			LocationToAim = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * (ShootDistance + CameraBoom->TargetArmLength));
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
	MovementVector = Value.Get<FVector2D>();
	
	GetCustomCharacterMovement()->SetDashDirectionVector(MovementVector);
	GetCustomCharacterMovement()->SetHorizontalJumpDirection(MovementVector);

	if(GetCustomCharacterMovement()->DoHorizontalJump()) return;
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);

		// get forward vector
		ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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

void AGreenOneCharacter::Dash()
{
	GetCustomCharacterMovement()->CustomDash();
}

void AGreenOneCharacter::CanRegenerate()
{
	if(Health >= MaxHealth)
		return;
	GetWorld()->GetTimerManager().SetTimer(TimerRegen, [=]()
	{
		IsCombatMode = false;
	},CoolDown, false);
}

void AGreenOneCharacter::Regenerate(float DeltaSeconds)
{
	if(IsCombatMode) return;
	if(Health < MaxHealth)
	{
		Health += 10*DeltaSeconds;
		if(Health >= MaxHealth)
		{
			Health = MaxHealth;
		}
		OnRegen.Broadcast();
	}
}
