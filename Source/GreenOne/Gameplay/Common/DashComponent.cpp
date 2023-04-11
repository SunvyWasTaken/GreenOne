// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "Kismet/KismetMathLibrary.h"

UDashComponent::UDashComponent()
{
	DashDistance = 1000.f;
	DashTime = 0.8f;
	DashCooldown = 2.f;
	bDashOnCooldown = false;
	bIsDashing = false;
}

void UDashComponent::Dash()
{
	if (Character == nullptr) { return; }
	
	if (Character->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Custom);
	StartDashLocation = GetActorLocation();
	TargetDashLocation = StartDashLocation + Character->GetActorForwardVector() * DashDistance;
	CurrentDashAlpha = 0.f;
	bIsDashing = true;
}

void UDashComponent::DashTick(float deltatime)
{
	if (!bIsDashing || bDashOnCooldown) { return; }

	if (Character == nullptr) { return; }

	CurrentDashAlpha += (1 / DashTime) * deltatime;
	if (CurrentDashAlpha >= 1)
	{
		CurrentDashAlpha = 1;
		CurrentDashCooldown = DashCooldown;
		bIsDashing = false;
		bDashOnCooldown = true;
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	FVector TargetLocation = UKismetMathLibrary::VLerp(StartDashLocation, TargetDashLocation, CurrentDashAlpha);
	Character->SetActorLocation(TargetLocation);
	return;
}

void UDashComponent::CooldownDash(float deltatime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= deltatime;
	if (CurrentDashCooldown <= 0.f)
	{
		bDashOnCooldown = false;
	}
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	DashTick(DeltaTime);
	CooldownDash(DeltaTime);
}
