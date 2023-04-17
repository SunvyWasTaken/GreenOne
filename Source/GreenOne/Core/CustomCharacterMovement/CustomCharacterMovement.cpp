// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovement.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

void UCustomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GreenOneCharacter = Cast<AGreenOneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DashTick(DeltaTime);
	CooldownTick(DeltaTime);


#if WITH_EDITOR && (DEBUG_MESSAGE_DASH == AVAILABLE)
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Blue, FString::Printf(TEXT("Dash CoolDown %f"), CurrentDashCooldown), true, FVector2d(1.5, 1.5));
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Red, FString::Printf(TEXT("Dash CoolDownValue %f"), DashCooldown), true, FVector2d(1.5, 1.5));
#endif
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UCustomCharacterMovementComponent::Dash()
{
	// Securite
	if (GC == nullptr) { return; }
	if (GC->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	// 

	GC->GetCharacterMovement()->SetMovementMode(MOVE_Custom, CMOVE_DASH);
	StartDashLocation = GC->GetActorLocation();

	FVector DirectionVector = FVector::ZeroVector;

	// Récupération de la direction du joueur
	FVector Direction = GC->GetActorForwardVector().GetSafeNormal2D();

	if (DashDirectionVector != FVector2D::ZeroVector )
	{
		FVector Forward = GC->GetActorForwardVector().GetSafeNormal2D() * DashDirectionVector.Y;
		FVector Right = GC->GetActorRightVector().GetSafeNormal2D() * DashDirectionVector.X;
		Direction = Forward + Right;
		Direction.Normalize();
	}

	Direction = FVector(Direction.X, Direction.Y, 0.f);

	if (Direction == FVector::ZeroVector)
	{
		DirectionVector = GC->GetActorForwardVector();
	}
	else
	{
		DirectionVector = Direction;
	}

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	TargetDashLocation = StartDashLocation + DirectionVector * DashDistance;

	// On verifie si le dash est en collision avec un objet
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDashLocation, TargetDashLocation, ECC_Visibility, CollisionParams);

	// Si le dash est en collision avec un objet, on reduit la distance du dash
	if (bIsHit)
	{
		TargetDashLocation = StartDashLocation + (DirectionVector * (HitResult.Distance - 50.f));
	}

	// DashTime
	DashTime = (DashDistance / DashSpeed) * 1000;
	CurrentDashAlpha = 0.f;
	bIsDashing = true;
}

void UCustomCharacterMovementComponent::DashTick(float DeltaTime)
{
	// Securite
	if (!bIsDashing || bDashOnCooldown) { return; }
	if (GreenOneCharacter == nullptr) { return; }
	//

	CurrentDashAlpha += (DeltaTime * 1000) / (DashTime);

	if (CurrentDashAlpha >= 1)
	{
		CurrentDashAlpha = 1;
		CurrentDashCooldown = DashCooldown;
		bIsDashing = false;
		bDashOnCooldown = true;
		GreenOneCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	FVector TargetLocation = UKismetMathLibrary::VLerp(StartDashLocation, TargetDashLocation, CurrentDashAlpha);
	GreenOneCharacter->SetActorLocation(TargetLocation);
	
}

void UCustomCharacterMovementComponent::CooldownTick(float DeltaTime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= DeltaTime;

	if (CurrentDashCooldown <= 0.f)
	{
		CurrentDashCooldown = 0.f;
		DashDirectionVector = FVector2D::ZeroVector;
		bDashOnCooldown = false;
	}
}