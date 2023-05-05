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

void UCustomCharacterMovementComponent::BackToPreviousPosition()
{
	if ( PreviousLocation != FVector::ZeroVector )
	{
		GetOwneChara()->SetActorLocation(PreviousLocation);
		PreviousLocation = FVector::ZeroVector;
	}
}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ExecHorizontalJump();
	ExecVerticalJump(DeltaTime);
	CustomDashTick(DeltaTime);
    CooldownTick(DeltaTime);
	// if ( IsStaticPosition(DeltaTime) ) { BackToPreviousPosition(); } // A AMELIORER // TODO : PAS OUF
}

void UCustomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (IsMovingOnGround() && !IsFalling())
	{
		InJumpState = JS_None;
		GetOwneChara()->JumpMaxCount = MaxJump;
		GravityScale = CustomGravityScale;
		bVerticalJump = false;
		bHorizontalJump = false;
		HorizontalJumpDirection = FVector2D::ZeroVector;
	}
}

void UCustomCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

#pragma region Jump/Falling

bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	bIsDashing = false;
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
	GetOwneChara()->JumpCurrentCount = 0;
	GetCharacterOwner()->JumpCurrentCountPreJump = 0;
	GetOwneChara()->JumpMaxCount = MaxJump + 1;
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
		Velocity = GetOwneChara()->GetVelocity();
		CurrentLocation = GetOwneChara()->GetActorLocation();
		bVerticalJump = true;
		TargetJumpLocation = CurrentLocation + FVector::UpVector * MaxVerticalHeight;
		JumpTime = 0;
		return true;
	}
	return false;
}

bool UCustomCharacterMovementComponent::HorizontalJump()
{
	if (bHorizontalJump) return false;

	bVerticalJump = false;
	FVector Direction = GetOwneChara()->GetActorForwardVector().GetSafeNormal2D();
	FVector Target = Direction;

	if (HorizontalJumpDirection != FVector2D::ZeroVector)
	{
		FVector Forward = GetOwneChara()->GetActorForwardVector().GetSafeNormal2D() * HorizontalJumpDirection.Y;
		FVector Right = GetOwneChara()->GetActorRightVector().GetSafeNormal2D() * HorizontalJumpDirection.X;
		Direction = Forward.GetSafeNormal() + Right.GetSafeNormal();
		Direction.Normalize();

		Target = Direction;
	}

	TempRotationCharacter = GetRotationToDirection(Direction);
	
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

		TargetJumpLocation = GetOwneChara()->GetActorLocation() + Direction * MaxDistanceHorizontalJump;

		FHitResult ObstacleHit;
		float CapsuleRadius = GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		TArray<AActor*> ActorsIgnores;
		ActorsIgnores.Push(GetOwneChara());

		//Check if the horizontalJump preview hit an obstacle
		bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingle(
			GetWorld(), GetOwneChara()->GetActorLocation(),
			TargetJumpLocation, CapsuleRadius, CapsuleHalfHeight,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			ActorsIgnores, EDrawDebugTrace::ForDuration, ObstacleHit, true, FLinearColor::Red, FLinearColor::Blue, 2);

		if (bObstacleHit)
		{
			TargetJumpLocation = ObstacleHit.ImpactPoint;
			DistanceHorizontalJump = ObstacleHit.Distance;
		}

		CurrentLocation = GetOwneChara()->GetActorLocation();
		GetOwneChara()->LaunchCharacter(Target, true, true);
		DrawDebugCapsule(GetWorld(), TargetJumpLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity,
		                 FColor::Purple, false, 3);

		bHorizontalJump = true;
		return true;
	}

	return false;
}

void UCustomCharacterMovementComponent::ExecVerticalJump(const float DeltaTime) {
	if (!bVerticalJump) return;

	bIsDashing = false;

	JumpTime += DeltaTime;

	const float CurveDeltaTime = (VelocityTemp / MaxVerticalHeight) * JumpTime;

	if (bActiveCheckRoof)
	{
		FHitResult VerticalJumpHitResult;
		const bool bVerticalJumpHit = GetWorld()->LineTraceSingleByChannel(
			VerticalJumpHitResult, GetOwneChara()->GetActorLocation(),
			GetOwneChara()->GetActorLocation() + FVector::UpVector * 3, ECC_Visibility);
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
	const float NewZLocation = UKismetMathLibrary::Ease(CurrentLocation.Z, TargetJumpLocation.Z, CurveDeltaTime, VerticalJumpCurve);
	GetOwneChara()->SetActorLocation(FVector(GetOwneChara()->GetActorLocation().X, GetOwneChara()->GetActorLocation().Y, NewZLocation), true);
}

void UCustomCharacterMovementComponent::ExecHorizontalJump()
{
	if (!bHorizontalJump) return;

	TargetDistance += FVector::Distance(CurrentLocation, GetOwneChara()->GetActorLocation());

	if (TargetDistance == 0.0f && !BlockCheckHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(BlockCheckHandle, [&]()
		{
			GetOwneChara()->LaunchCharacter(GetOwneChara()->GetForwardDirection() * -150.f, false, false);
		},DelayToBlockCheck,false);	
		
	}
	else if(TargetDistance > 0.0f)
	{
		BlockCheckHandle.Invalidate();
	}
	
	if (TargetDistance > DistanceHorizontalJump)
	{
		bHorizontalJump = false;
		GravityScale = CustomGravityScale;
		HorizontalJumpDirection = FVector2D::ZeroVector;
		TargetDistance = 0;
		InJumpState = JS_None;
	}
	CurrentLocation = GetActorLocation();
	GetOwneChara()->SetActorRotation(TempRotationCharacter);
}

FRotator UCustomCharacterMovementComponent::GetRotationToDirection(FVector Direction)
{
	return FRotator( 0.f, Direction.Rotation().Yaw, 0.f);
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

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}

#pragma endregion

#pragma region Dash

void UCustomCharacterMovementComponent::CooldownTick(float DeltaTime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= DeltaTime;

	if (CurrentDashCooldown <= 0.f)
	{
		CurrentDashCooldown = 0.f;
		bDashOnCooldown = false;
	}
}

void UCustomCharacterMovementComponent::CustomDash()
{
	if (IsFalling() && GetCurrentJumpState() == JS_Vertical)
	{
		GetOwneChara()->Jump();
		return;
	}
	if (bDashOnCooldown || bIsDashing) { return; }
	if (IsToClose()) { return; }

	CurrentLocation = GetOwneChara()->GetActorLocation();
	CustomForwardVector = GetOwneChara()->GetActorForwardVector();
	if (DashDirectionVector2D != FVector2D::ZeroVector)
	{
		CustomForwardVector *= DashDirectionVector2D.Y;
		CustomForwardVector += GetOwneChara()->GetActorRightVector() * DashDirectionVector2D.X;
		CustomForwardVector.Normalize();
	}
	CustomEndLocation = CurrentLocation + (CustomForwardVector * CustomDashDistance);
	CustomTraceParcourtDistance = 0.f;

	StartLocation = CurrentLocation;
	TheoricEndLocation = CurrentLocation + (CustomForwardVector * CustomDashDistance);
	
	PreviousVel = Velocity.Length();
	bIsDashing = true;
	CurrentCustomDashDistance = CustomDashDistance;
}

void UCustomCharacterMovementComponent::StopDash()
{
	bIsDashing = false;
	DashDirectionVector2D = FVector2D::ZeroVector;
	bDashOnCooldown = true;
	CurrentDashCooldown = DashCooldown;
}

bool UCustomCharacterMovementComponent::CheckTheoricPosition()
{
	if ( TheoricEndLocation == FVector::ZeroVector ) { return false; }
	if ( CurrentLocation == TheoricEndLocation ) { return false;	}

	float const_between = FVector::DistXY(StartLocation, TheoricEndLocation);
	float d_between = FVector::DistXY(CurrentLocation, TheoricEndLocation);

	if ( d_between <= const_between ) { return true; }
	
	return false;
}

/** TODO : PAS OUF 
bool UCustomCharacterMovementComponent::IsStaticPosition(float DeltaTime)
{
	const FVector CustomCurrentLocation = GetOwneChara()->GetActorLocation();

	if (PreviousLocation == FVector::Zero())
	{
		return false;
	}
	
	if (PreviousLocation == CustomCurrentLocation)
	{
		CurrentStaticPositionTime += DeltaTime;
		if ( CurrentStaticPositionTime >= StaticPositionMaxTime)
		{
			CurrentStaticPositionTime = 0.f;
			return true;
		}
	}

	return false;
}
**/

void UCustomCharacterMovementComponent::CustomDashTick(float Deltatime)
{
	if (!bIsDashing) { return; }

	TArray<AActor*> CustomActorToIgnore;
	CustomActorToIgnore.Add(GetOwner());

	const float CustomCapRadius = GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CustomCapHeight = GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector CustomDashDirection = CustomForwardVector * (Deltatime * (CustomDashSpeed + PreviousVel));
	Velocity = CustomDashDirection;
	CustomEndLocation = CurrentLocation + CustomDashDirection;

	float temp_distance = 0.f;
	if ( InFrontOfWall(&temp_distance) ) { CurrentCustomDashDistance = temp_distance; }

	if (CheckTheoricPosition())
	{
		if (CustomTraceParcourtDistance <= CurrentCustomDashDistance - 25.f)
		{
			FHitResult CustomCurrentOuthit;
			bool bHasHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CurrentLocation, CustomEndLocation, CustomCapRadius, CustomCapHeight,
				UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, CustomActorToIgnore, EDrawDebugTrace::None, CustomCurrentOuthit, true);

			if (bHasHit)
			{
				CustomTraceParcourtDistance += CustomCurrentOuthit.Distance;
				CurrentLocation = CustomCurrentOuthit.Location + CustomCurrentOuthit.ImpactNormal;
				CustomForwardVector = FVector::VectorPlaneProject(CustomForwardVector, CustomCurrentOuthit.ImpactNormal);
				CustomForwardVector.Normalize();
			}
			else
			{
				CustomTraceParcourtDistance += CustomDashDirection.Length();
				CurrentLocation = CustomCurrentOuthit.TraceEnd;
			}
			GetOwner()->AddActorWorldOffset(CustomDashDirection);
			GetOwner()->SetActorRotation(GetRotationToDirection(CustomDashDirection));
		}
		else
		{
			StopDash();
		}
	}
	else
	{
		StopDash();
	}
}

bool UCustomCharacterMovementComponent::IsToClose()
{
	float _distance = 50.f;
	if (InFrontOfWall(&_distance))
	{
		if (_distance <= 25.f) { return true; }
	}
	return false;
}

bool UCustomCharacterMovementComponent::InFrontOfWall(float* Distance)
{
	const float CustomCapRadius = GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CustomCapHeight = ( GetOwneChara()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2 );

	TArray<AActor*> CustomActorToIgnore;
	CustomActorToIgnore.Add(GetOwner());
	
	FHitResult WallHitResult;
	bool bHasHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CurrentLocation, CustomEndLocation, CustomCapRadius, CustomCapHeight,
		UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, CustomActorToIgnore, EDrawDebugTrace::None, WallHitResult, true);

	if (bHasHit)
	{
		Distance = &WallHitResult.Distance;
		return true;
	}
	else
		return false;
}

#pragma endregion
