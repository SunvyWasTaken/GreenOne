// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

#include "GreenOne/Gameplay/GreenOneCharacter.h"

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

void UCustomCharacterMovementComponent::JumpAction(const FInputActionValue& Value)
{
	bool bIsJumping = Value.Get<bool>();
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
		
	}
	
	return false;
	//return Super::DoJump(bReplayingMoves);
}