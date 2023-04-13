// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

DECLARE_CYCLE_STAT(TEXT("Char PhysFalling"), STAT_CharPhysFalling, STATGROUP_Character);

namespace CharacterMovementCVars
{
	static int32 UseTargetVelocityOnImpact = 1;
	FAutoConsoleVariableRef CVarUseTargetVelocityOnImpact(
		TEXT("p.UseTargetVelocityOnImpact"),
		UseTargetVelocityOnImpact, TEXT("When disabled, we recalculate velocity after impact by comparing our position before we moved to our position after we moved. This doesn't work correctly when colliding with physics objects, so setting this to 1 fixes this one the hit object is moving."));

	int32 ForceJumpPeakSubstep = 1;
	FAutoConsoleVariableRef CVarForceJumpPeakSubstep(
		TEXT("p.ForceJumpPeakSubstep"),
		ForceJumpPeakSubstep,
		TEXT("If 1, force a jump substep to always reach the peak position of a jump, which can often be cut off as framerate lowers."),
		ECVF_Default);
	
}

namespace CustomCharacterMovementConstants
{
	// MAGIC NUMBERS
	const float MAX_STEP_SIDE_Z = 0.08f;	// maximum z value for the normal on the vertical side of steps
	const float SWIMBOBSPEED = -80.f;
	const float VERTICAL_SLOPE_NORMAL_Z = 0.001f; // Slope is vertical if Abs(Normal.Z) <= this threshold. Accounts for precision problems that sometimes angle normals slightly off horizontal for vertical surface.
}

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
	UE_LOG(LogTemp, Warning, TEXT("DoJump"));
	if(!IsFalling())
		JumpCount = 0;
	
	JumpCount++;
	
	if (CharacterOwner && CharacterOwner->CanJump() )
	{
		if(JumpCount == 1)
			return VerticalJump();
		if(JumpCount == 2)
			return HorizontalJump();
		
	}
	
	return false;
	//return Super::DoJump(bReplayingMoves);
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

	UE_LOG(LogTemp, Warning, TEXT("HorizontalJumpDirection = X : %f, Y : %f"),HorizontalJumpDirection.X,HorizontalJumpDirection.Y);
	
	if(HorizontalJumpDirection != FVector2D::ZeroVector)
	{
		FVector Forward = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D() * HorizontalJumpDirection.Y;
		FVector Right = GetOwnerCharacter()->GetActorRightVector().GetSafeNormal2D() * HorizontalJumpDirection.X;
		Direction = Forward.GetSafeNormal() + Right.GetSafeNormal();
		Direction.Normalize();

		UE_LOG(LogTemp, Warning, TEXT("Forward = X : %f, Y : %f, Z : %f"),GetOwnerCharacter()->GetOwnerFollowCamera()->GetForwardVector().GetSafeNormal2D().X,GetOwnerCharacter()->GetOwnerFollowCamera()->GetForwardVector().GetSafeNormal2D().Y, Forward.Z);
		UE_LOG(LogTemp, Warning, TEXT("Right = X : %f, Y : %f, Z : %f"),Right.X,Right.Y, Right.Z);
		UE_LOG(LogTemp, Warning, TEXT("Direction = X : %f, Y : %f, Z : %f"),Direction.X,Direction.Y, Direction.Z);
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
	}
	CurrentLocation = GetActorLocation();
}

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}