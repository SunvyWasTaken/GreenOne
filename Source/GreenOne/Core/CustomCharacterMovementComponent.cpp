// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovementComponent.h"

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}
