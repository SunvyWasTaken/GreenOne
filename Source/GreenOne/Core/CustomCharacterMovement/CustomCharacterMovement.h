// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEFINE.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovement.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_NONE	UMETA(Hidden),
	//TODO: Add Custom movement mode
	CMOVE_DASH  UMETA(DisplayName = "Dash"),
	CMOVE_MAX	UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class GREENONE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	class AGreenOneCharacter* GreenOneCharacter;

protected:
	virtual void InitializeComponent() override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;


#pragma region Dash
public:

	// Dash dans la direction de l'input mouvement.
	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
		void Dash();

	// Distance du dash
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Distance du dash", ClampMin = 0), Category = "Custom|Dash")
		float DashDistance = 1000.f; // CM  => 100cm = 1m | 1000cm = 10m

	// Le temps que va prendre le dash pour attendre ca destination.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps du dash", ClampMin = 0), Category = "Custom|Dash")
		float DashTime = 1.0f; // s

	// Temps que va prendre le dash a revenir apres utilisation.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps de recharge du Dash"), Category = "Custom|Dash")
		float DashCooldown = 3.0f; // S

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "IsDashing"), Category = "Custom|Dash")
		bool bIsDashing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Dash")
		bool bDashOnCooldown = false;

	/**
	 * Return the remaining time of the dash cooldown.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Keywords = "Cooldown|Dash"), Category = "Dash")
		float GetRemainingDashTime() { return CurrentDashCooldown; };

private:

	// Utiliser pour placer le player pendant le Dash
	void DashTick(float deltatime);

	// Cooldown du Dash
	void CooldownTick(float deltatime);

	FVector TargetDashLocation;

	FVector StartDashLocation;

	float CurrentDashAlpha;

	float CurrentDashCooldown;

#pragma endregion Dash


};
