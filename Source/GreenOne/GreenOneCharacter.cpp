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

//////////////////////////////////////////////////////////////////////////
// AGreenOneCharacter

AGreenOneCharacter::AGreenOneCharacter()
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
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ShootCooldown = 1.f/3.f;
	ShootBloom = 0.f;
	CanShoot = true;

	DashDistance = 100.f;
	DashTime = 0.5f;
	DashCooldown = 3.f;
	bDashOnCooldown = false;
	bIsDashing = false;
}

void AGreenOneCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGreenOneCharacter, Health))
	{
		MaxHealth = Health;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGreenOneCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGreenOneCharacter::InputJump);
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
	MaxHealth = Health;
	ShootCooldownRemaining = ShootCooldown;
}

void AGreenOneCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ShootTick(DeltaSeconds);
	DashTick(DeltaSeconds);
	CooldownDash(DeltaSeconds);
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

void AGreenOneCharacter::EntityTakeDamage_Implementation(float damage, FName BoneNameHit, AActor* DamageSource = nullptr)
{
	Health -= damage;
	if(Health <= 0)
	{
		PlayerDead();
		Health = 0.f;
	}
	OnTakeDamage.Broadcast();
}

float AGreenOneCharacter::GetHealthPercent()
{
	return Health/MaxHealth;
}

void AGreenOneCharacter::Shoot()
{

	if (!CanShoot) { return; }

	CanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(ShootHandler, this, &AGreenOneCharacter::ShootRafale, ShootCooldown, true);
	ShootRafale();
}

void AGreenOneCharacter::StopShoot()
{
	if (ShootHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandler);
	}
}

void AGreenOneCharacter::ShootRafale()
{
	APlayerCameraManager* CameraRef = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FHitResult OutHit;

	float DegreeRotation = UKismetMathLibrary::Lerp(0.f, 360.f, ShootBloom);
	FVector StartLocation = CameraRef->GetCameraLocation();
	FVector EndLocation = StartLocation + UKismetMathLibrary::RandomUnitVectorInConeInDegrees(CameraRef->GetActorForwardVector(), DegreeRotation) * ShootDistance;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Camera))
	{
		if (DotDecal)
		{
			GetWorld()->SpawnActor<AActor>(DotDecal, OutHit.Location, OutHit.Normal.Rotation());
		}
		if (OutHit.GetActor())
		{
			if (OutHit.GetActor()->Implements<UEntityGame>())
			{
				IEntityGame::Execute_EntityTakeDamage(OutHit.GetActor(), DamagePlayer, OutHit.BoneName, this);
				OnHitEnnemy.Broadcast();
			}
		}
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
}

void AGreenOneCharacter::Dash()
{
	if(bDashOnCooldown || bIsDashing) { return; }
	GetCharacterMovement()->SetMovementMode(MOVE_Custom);
	StartDashLocation = GetActorLocation();
	TargetDashLocation = StartDashLocation + GetActorForwardVector() * DashDistance;
	CurrentDashAlpha = 0.f;
	bIsDashing = true;
}

void AGreenOneCharacter::DashTick(float deltatime)
{
	if (!bIsDashing || bDashOnCooldown) { return; }

	CurrentDashAlpha += (1/DashTime) * deltatime;
	if (CurrentDashAlpha >= 1)
	{
		CurrentDashAlpha = 1;
		CurrentDashCooldown = DashCooldown;
		bIsDashing = false;
		bDashOnCooldown = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	FVector TargetLocation = UKismetMathLibrary::VLerp(StartDashLocation, TargetDashLocation, CurrentDashAlpha);
	SetActorLocation(TargetLocation);
	return;
}

void AGreenOneCharacter::CooldownDash(float deltatime)
{
	if(!bDashOnCooldown) { return; }
	CurrentDashCooldown -= deltatime;
	if (CurrentDashCooldown <= 0.f)
	{
		bDashOnCooldown = false;
	}
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
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
