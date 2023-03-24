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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Custom|Tire")
	void Shoot();

	/**
	 * Correspond au nombre de tire par seconde
	 * 2 = 2 tire par seconde
	 * 1 = 1 tire par seconde
	 * 0.5 = 1 toutes les 2 secondes
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, UIMin = 0, UIMax = 5), Category = "Custom|Tire")
	float ShootRate = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Tire")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Tire")
	float ShootRange = 5000.f;

private:

	void ActiveCooldown();

	void TickCooldown(float DeltaSeconds);

	bool IsInCooldown = false;

	float TimeRemainingForShoot = 0;

	void TimerShoot();

	FTimerHandle ShootTimer;

};
