// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_NONE	UMETA(Hidden),
	//TODO: Add Custom movement mode
	CMOVE_MAX	UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class GREENONE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_CustomCharacter : public FSavedMove_Character
	{
	public:
		FSavedMove_CustomCharacter();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;

		//Walk Speed Update
		uint8 bSavedRequestMaxWalkSpeedChange : 1;
	};

	UPROPERTY(Transient)
	AGreenOneCharacter* GreenOneCharacter;

protected:
	virtual void InitializeComponent() override;
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
};
