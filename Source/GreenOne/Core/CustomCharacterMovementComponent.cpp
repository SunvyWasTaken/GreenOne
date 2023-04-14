// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CustomGravityScale = GravityScale;
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

bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	
	if(IsMovingOnGround() && !IsFalling())
	{
		JumpCount = 0;
		GetOwnerCharacter()->JumpMaxCount = MaxJump;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("DoJump %d"),JumpCount);
	
	JumpCount++;

	UE_LOG(LogTemp, Warning, TEXT("DoJump %d"),JumpCount);
	
	if (CharacterOwner && CharacterOwner->CanJump() )
	{
		bool bJumped = false;
		if(JumpCount == 1)
		{
			bJumped = VerticalJump();
		}else if(JumpCount == 2)
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
	JumpCount = 0;
	return Super::CheckFall(OldFloor, Hit, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump);
}

bool UCustomCharacterMovementComponent::VerticalJump()
{
	UE_LOG(LogTemp, Warning, TEXT("VerticalJump"));
	// Don't jump if we can't move up/down.
	if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
	{
		//Velocity.Z = VerticalJumpVelocity;
		AddImpulse(FVector::UpVector * VerticalJumpVelocity, true);
		GravityScale = FallingGravity;
		SetMovementMode(MOVE_Falling);

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
	
	if(bManualHorizontalVelocity)
	{
		Target *= HorizontalJumpVelocity;
	}else
	{
		Target *= JumpZVelocity;
	}
	
	if(JumpCount == 2 && IsFalling())
	{
		DistanceHorizontalJump = MaxDistanceHorizontalJump;
		GravityScale = 0.f;

		TargetHorizontalJump = GetOwnerCharacter()->GetActorLocation() + Direction * MaxDistanceHorizontalJump;
		
		FHitResult ObstacleHit;
		float CapsuleRadius = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		TArray<AActor*> ActorsIgnores;
		ActorsIgnores.Push(GetOwnerCharacter());
		
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

bool UCustomCharacterMovementComponent::DoHorizontalJump() const
{
	return bHorizontalJump;
}

int32 UCustomCharacterMovementComponent::GetJumpCount() const
{
	return JumpCount;
}


void UCustomCharacterMovementComponent::ExecHorizontalJump()
{
	if(!bHorizontalJump) return;

	TargetDistance += FVector::Distance(CurrentLocation, GetOwnerCharacter()->GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("Distance %f"), TargetDistance);
	
	if(TargetDistance > DistanceHorizontalJump)
	{
		bHorizontalJump = false;
		GravityScale = CustomGravityScale;
		// = FVector2D::ZeroVector;
		TargetDistance = 0;
		JumpCount = 0;
	}
	CurrentLocation = GetActorLocation();
}

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}