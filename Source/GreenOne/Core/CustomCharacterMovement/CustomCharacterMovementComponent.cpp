// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DashDistance = 1000.f;
	DashTime = 0.8f;
	DashCooldown = 2.f;
	bDashOnCooldown = false;
	bIsDashing = false;
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

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GreenOneCharacter = Cast<AGreenOneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DashTick(DeltaTime);
	CooldownDash(DeltaTime);
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UCustomCharacterMovementComponent::Dash()
{
	if (GreenOneCharacter == nullptr) { return; }

	if (GreenOneCharacter->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	GreenOneCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Custom);
	StartDashLocation = GetActorLocation();
	TargetDashLocation = StartDashLocation + GreenOneCharacter->GetActorForwardVector() * DashDistance;
	CurrentDashAlpha = 0.f;
	bIsDashing = true;
}

void UCustomCharacterMovementComponent::DashTick(float deltatime)
{
	if (!bIsDashing || bDashOnCooldown) { return; }

	if (GreenOneCharacter == nullptr) { return; }

	CurrentDashAlpha += (1 / DashTime) * deltatime;
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
	return;
}

void UCustomCharacterMovementComponent::CooldownDash(float deltatime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= deltatime;
	if (CurrentDashCooldown <= 0.f)
	{
		bDashOnCooldown = false;
	}
}