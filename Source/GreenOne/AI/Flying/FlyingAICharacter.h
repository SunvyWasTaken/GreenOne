// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "FlyingAICharacter.generated.h"

UCLASS()
class GREENONE_API AFlyingAICharacter : public ABaseEnnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

#pragma region Property



#pragma endregion

#pragma region Tire

public:

	// Percentage de degats du tire en percentage en fonction de l'attaque de l'ennemie de base.
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Damage du Tire", ClampMin = 0, UIMin = 0, UIMax = 1), Category = "Custom|Tire")
	float RatioDmgShoot = 1.f;

	UFUNCTION(BlueprintCallable, Category = "Custom|Tire")
	void Shoot();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanShoot() { return !IsInCooldown; };

	/**
	 * Correspond au nombre de tire par seconde
	 * 2 = 2 tire par seconde
	 * 1 = 1 tire par seconde
	 * 0.5 = 1 toutes les 2 secondes
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, UIMin = 0, UIMax = 5), Category = "Custom|Tire")
	float ShootRate = 0.5f;

	/**
	 * Use the old fashon trace to shoot.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Tire|UseTrace")
	bool bUseTrace = false;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Tire")
	TSubclassOf<class AAIProjectil> ProjectileClass;

	/**
	 * Definit une distance de tire lorsqu'utilisation du trace.
	 */
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bUseTrace), Category = "Custom|Tire|UseTrace")
	float ShootRange = 5000.f;

#pragma endregion

	UFUNCTION(BlueprintCallable)
	virtual void UpdateMaxSpeed(float Speed) override;
	virtual void ResetEffect(const float DelayToReset) override;

#pragma region AnimationRotation

	/**
	 * Vitesse de rotation de l'animation des drones.
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Custom|Animation")
	float RotationSpeed = 5.f;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FVector2D GetRotationAxis() { return CurrentRotationInput; };

	void SetRotationAxis(FVector2D TargetAxis);

#pragma endregion

#pragma region Explosion

	UFUNCTION(BlueprintCallable, Category = "Custom|Explosion")
	void SelfDestruction();

	UPROPERTY(EditAnywhere, Category = "Custom|Explosion")
	class UNiagaraSystem* ExplosionParticule;

	/**
	 * Radius de l'explosion.
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (DisplayName = "Radius d'explosion", ClampMin = 0), Category = "Custom|Explosion")
	float ExploRadius;

	/**
	 * Percentage damage of the explosion
	 */
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Damage de l'explosion", ClampMin = 0, UIMin = 0, UIMax = 1), Category = "Custom|Explosion")
	float RatioExploDmg = 1.8f;

#pragma endregion 

private:

#pragma region Tire

	float ShootDmg;

	float ExploDmg;

	// Activate the cooldown of the shoot.
	void ActiveCooldown();

	// Function use to count the cooldown.
	void TickCooldown(float DeltaSeconds);

	bool IsInCooldown = false;

	float TimeRemainingForShoot = 0;

	void TimerShoot();

	FTimerHandle ShootTimer;

#pragma endregion

	void TickRotation(float DeltaSeconds);
	
	FVector2D CurrentRotationInput;

	FVector2D TargetRotationInput;

};
