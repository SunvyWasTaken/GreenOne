// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
		
public:

	UCustomCharacterMovementComponent();
	
	// Dash dans la direction de l'input mouvement.
	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
		void Dash();

	// Distance du dash
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Vitesse du dash", ClampMin = 0), Category = "Custom|Dash")
		float DashDistance;

	// Le temps que va prendre le dash pour attendre ça destination.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps du dash", ClampMin = 0), Category = "Custom|Dash")
		float DashTime;

	// Temps que va prendre le dash à revenir après utilisation.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps de recharge du Dash"), Category = "Custom|Dash")
		float DashCooldown;

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "IsDashing"), Category = "Custom|Dash")
		bool bIsDashing;

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Dash")
		bool bDashOnCooldown;

	/**
	 * Return the remaining time of the dash cooldown.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Keywords = "Cooldown|Dash"), Category = "Dash")
		float GetRemainingDashTime() { return CurrentDashCooldown; };

private:

	// Utiliser pour placer le player pendant le Dash
	void DashTick(float deltatime);

	void CooldownDash(float deltatime);

	FVector TargetDashLocation;

	FVector StartDashLocation;

	float CurrentDashAlpha;

	float CurrentDashCooldown;

	class AGreenOneCharacter* Character;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
