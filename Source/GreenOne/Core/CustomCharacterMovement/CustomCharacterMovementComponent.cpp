// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GravityScale = CustomGravityScale;
	JumpZVelocity = JumpVelocity;
	MaxDistanceHorizontalJump = JumpZVelocity / 2;
}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ExecHorizontalJump();
	ExecVerticalJump(DeltaTime);
    
    DashTick(DeltaTime);
    CooldownTick(DeltaTime);    
    
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

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
                                                              uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (IsMovingOnGround() && !IsFalling())
	{
		InJumpState = JS_None;
		GetOwnerCharacter()->JumpMaxCount = MaxJump;
		GravityScale = CustomGravityScale;
		bVerticalJump = false;
		bHorizontalJump = false;
		HorizontalJumpDirection = FVector2D::ZeroVector;
	}
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

#pragma region Jump/Falling
bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		bool bJumped = false;
		//Check the jumpState
		if (InJumpState == JS_None)
		{
			bJumped = VerticalJump();
		}
		else if (InJumpState == JS_Vertical)
		{
			bJumped = HorizontalJump();
		}

		return bJumped;
	}

	return false;
}

bool UCustomCharacterMovementComponent::CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit,
                                                  const FVector& Delta, const FVector& OldLocation, float remainingTime,
                                                  float timeTick, int32 Iterations,
                                                  bool bMustJump)
{
	GetOwnerCharacter()->JumpCurrentCount = 0;
	GetCharacterOwner()->JumpCurrentCountPreJump = 0;
	GetOwnerCharacter()->JumpMaxCount = MaxJump + 1;
	InJumpState = JS_None;
	return Super::CheckFall(OldFloor, Hit, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump);
}

bool UCustomCharacterMovementComponent::VerticalJump()
{
	//Check if the vertical Jump is edit manually

	if (bManualVerticalVelocity)
	{
		VelocityTemp = VerticalJumpVelocity;
	}
	else
	{
		VelocityTemp = JumpZVelocity;
	}

	if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
	{
		SetMovementMode(MOVE_Falling);
		InJumpState = JS_Vertical;
		bVerticalJump = true;
		TargetJumpLocation = GetOwnerCharacter()->GetActorLocation() + FVector::UpVector * MaxVerticalHeight;
		CurrentLocation = GetOwnerCharacter()->GetActorLocation();
		JumpTime = 0;
		return true;
	}
	return false;
}

bool UCustomCharacterMovementComponent::HorizontalJump()
{
	if (bHorizontalJump) return false;

	bVerticalJump = false;
	FVector Direction = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D();
	FVector Target = Direction;

	if (HorizontalJumpDirection != FVector2D::ZeroVector)
	{
		FVector Forward = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D() * HorizontalJumpDirection.Y;
		FVector Right = GetOwnerCharacter()->GetActorRightVector().GetSafeNormal2D() * HorizontalJumpDirection.X;
		Direction = Forward.GetSafeNormal() + Right.GetSafeNormal();
		Direction.Normalize();

		Target = Direction;
	}

	//Check if the horizontal Jump is edit manually
	if (bManualHorizontalVelocity)
	{
		Target *= HorizontalJumpVelocity;
	}
	else
	{
		Target *= JumpZVelocity;
	}

	if (InJumpState == JS_Vertical && IsFalling())
	{
		DistanceHorizontalJump = MaxDistanceHorizontalJump;
		GravityScale = 0.f;

		TargetJumpLocation = GetOwnerCharacter()->GetActorLocation() + Direction * MaxDistanceHorizontalJump;

		FHitResult ObstacleHit;
		float CapsuleRadius = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		TArray<AActor*> ActorsIgnores;
		ActorsIgnores.Push(GetOwnerCharacter());

		//Check if the horizontalJump preview hit an obstacle
		bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingle(
			GetWorld(), GetOwnerCharacter()->GetActorLocation(),
			TargetJumpLocation, CapsuleRadius, CapsuleHalfHeight,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			ActorsIgnores, EDrawDebugTrace::ForDuration, ObstacleHit, true, FLinearColor::Red, FLinearColor::Blue, 2);

		if (bObstacleHit)
		{
			TargetJumpLocation = ObstacleHit.ImpactPoint;
			DistanceHorizontalJump = ObstacleHit.Distance;
		}

		CurrentLocation = GetOwnerCharacter()->GetActorLocation();
		GetOwnerCharacter()->LaunchCharacter(Target, true, true);
		DrawDebugCapsule(GetWorld(), TargetJumpLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity,
		                 FColor::Purple, false, 3);

		bHorizontalJump = true;
		return true;
	}

	return false;
}

/**
 * Return if Character can jump horizontally
 */
bool UCustomCharacterMovementComponent::DoHorizontalJump() const
{
	return bHorizontalJump;
}

EJumpState UCustomCharacterMovementComponent::GetCurrentJumpState() const
{
	return InJumpState;
}

void UCustomCharacterMovementComponent::ExecHorizontalJump()
{
	if (!bHorizontalJump) return;

	TargetDistance += FVector::Distance(CurrentLocation, GetOwnerCharacter()->GetActorLocation());

	if (TargetDistance > DistanceHorizontalJump)
	{
		bHorizontalJump = false;
		GravityScale = CustomGravityScale;
		HorizontalJumpDirection = FVector2D::ZeroVector;
		TargetDistance = 0;
		InJumpState = JS_None;
	}
	CurrentLocation = GetActorLocation();
}

void UCustomCharacterMovementComponent::ExecVerticalJump(const float DeltaTime)
{
	if (!bVerticalJump) return;

	JumpTime += DeltaTime;

	const float CurveDeltaTime = (VelocityTemp / MaxVerticalHeight) * JumpTime;

	if (bActiveCheckRoof)
	{
		FHitResult VerticalJumpHitResult;
		const bool bVerticalJumpHit = GetWorld()->LineTraceSingleByChannel(
			VerticalJumpHitResult, GetOwnerCharacter()->GetActorLocation(),
			GetOwnerCharacter()->GetActorLocation() + FVector::UpVector * 3, ECC_Visibility);
		if (bVerticalJumpHit)
		{
			DrawDebugSphere(GetWorld(), VerticalJumpHitResult.ImpactPoint, 8, 10, FColor::Red, false, 2);
			TargetJumpLocation.Z = VerticalJumpHitResult.ImpactPoint.Z;
		}
	}

	float NewZVelocity = UKismetMathLibrary::Ease(VerticalJumpVelocity, 0, CurveDeltaTime, VerticalJumpCurve);
	Velocity.Z = NewZVelocity;
	if (CurveDeltaTime > SafeZone)
	{
		bVerticalJump = false;
		//Velocity.Z = 0.f;
		return;
	}
	const float NewZLocation = UKismetMathLibrary::Ease(CurrentLocation.Z, TargetJumpLocation.Z, CurveDeltaTime,
	                                                    VerticalJumpCurve);
	GetOwnerCharacter()->SetActorLocation(FVector(GetOwnerCharacter()->GetActorLocation().X,
	                                              GetOwnerCharacter()->GetActorLocation().Y, NewZLocation), true);
}

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}
#pragma endregion

#pragma region Dash

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

#pragma endregion