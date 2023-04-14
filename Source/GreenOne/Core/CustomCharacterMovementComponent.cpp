// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CustomGravityScale = GravityScale;
	JumpZVelocity = JumpVelocity;
	MaxDistanceHorizontalJump = JumpZVelocity/2;
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
	if(IsMovingOnGround() && !IsFalling())
	{
		InJumpState = JS_Vertical;
		GetOwnerCharacter()->JumpMaxCount = MaxJump;
	}
	
	if (CharacterOwner && CharacterOwner->CanJump() )
	{
		bool bJumped = false;
		//Check the jumpState
		if(InJumpState == JS_Vertical)
		{
			bJumped = VerticalJump();
		}else if(InJumpState == JS_Horizontal)
		{
			bJumped = HorizontalJump();
		}
		
		return bJumped;
	}
	
	return false;
}

bool UCustomCharacterMovementComponent::CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit,
	const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations,
	bool bMustJump)
{
	GetOwnerCharacter()->JumpCurrentCount = 0;
	GetCharacterOwner()->JumpCurrentCountPreJump = 0;
	GetOwnerCharacter()->JumpMaxCount = MaxJump+1;
	InJumpState = JS_Vertical;
	return Super::CheckFall(OldFloor, Hit, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump);
}

bool UCustomCharacterMovementComponent::VerticalJump()
{
	//Check if the vertical Jump is edit manually
	float VelocityTemp;
	if(bManualVerticalVelocity)
	{
		VelocityTemp = VerticalJumpVelocity;
	}else
	{
		VelocityTemp = JumpZVelocity;
	}
	
	if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
	{
		AddImpulse(FVector::UpVector * VelocityTemp, true);
		GravityScale = FallingGravity;
		SetMovementMode(MOVE_Falling);
		InJumpState = JS_Horizontal;
		return true;
	}
	return false;
}

bool UCustomCharacterMovementComponent::HorizontalJump()
{
	if(bHorizontalJump) return false;
	
	FVector Direction = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D();
	FVector Target = Direction;
	
	if(HorizontalJumpDirection != FVector2D::ZeroVector)
	{
		FVector Forward = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D() * HorizontalJumpDirection.Y;
		FVector Right = GetOwnerCharacter()->GetActorRightVector().GetSafeNormal2D() * HorizontalJumpDirection.X;
		Direction = Forward.GetSafeNormal() + Right.GetSafeNormal();
		Direction.Normalize();
		
		Target = Direction;
	}

	//Check if the horizontal Jump is edit manually
	if(bManualHorizontalVelocity)
	{
		Target *= HorizontalJumpVelocity;
	}else
	{
		Target *= JumpZVelocity;
	}
	
	if(InJumpState == JS_Horizontal && IsFalling())
	{
		DistanceHorizontalJump = MaxDistanceHorizontalJump;
		GravityScale = 0.f;

		TargetHorizontalJump = GetOwnerCharacter()->GetActorLocation() + Direction * MaxDistanceHorizontalJump;
		
		FHitResult ObstacleHit;
		float CapsuleRadius = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		TArray<AActor*> ActorsIgnores;
		ActorsIgnores.Push(GetOwnerCharacter());

		//Check if the horizontalJump preview hit an obstacle
		bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),GetOwnerCharacter()->GetActorLocation(),
			TargetHorizontalJump, CapsuleRadius,CapsuleHalfHeight,UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),false,
			ActorsIgnores,EDrawDebugTrace::ForDuration,ObstacleHit,true,FLinearColor::Red,FLinearColor::Blue,2);

		if(bObstacleHit)
		{
			TargetHorizontalJump = ObstacleHit.ImpactPoint;
			DistanceHorizontalJump = ObstacleHit.Distance;
		}
		
		CurrentLocation = GetOwnerCharacter()->GetActorLocation();
		GetOwnerCharacter()->LaunchCharacter(Target,true, true);
		DrawDebugCapsule(GetWorld(), TargetHorizontalJump, CapsuleHalfHeight ,CapsuleRadius, FQuat::Identity, FColor::Purple, false, 3);
		
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
	if(!bHorizontalJump) return;

	TargetDistance += FVector::Distance(CurrentLocation, GetOwnerCharacter()->GetActorLocation());
	
	if(TargetDistance > DistanceHorizontalJump)
	{
		bHorizontalJump = false;
		GravityScale = CustomGravityScale;
		HorizontalJumpDirection = FVector2D::ZeroVector;
		TargetDistance = 0;
		InJumpState = JS_Vertical;
	}
	CurrentLocation = GetActorLocation();
}

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}
#pragma endregion 
