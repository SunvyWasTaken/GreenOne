// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/CrowdFollowingComponent.h"
#include "CoreMinimal.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "GameFramework/Character.h"
#include "MeleeAICharacter.generated.h"

UCLASS()
class GREENONE_API AMeleeAICharacter : public ABaseEnnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere)
	FVector Offset = FVector(0, 0, 0);
	
	UFUNCTION(BlueprintCallable)
	void Collision();
	





private:
	
	/** Radius de la zone de detection */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Detection", DisplayName = "Radius de la zone de détection")
	float DetectionRadius = 80.f;


	/** Offset de la detection selon la position du parent */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Detection", DisplayName = "Offset de la zone de détection")
	FVector DetectionOffset;


	// blabla
	
	//UFUNCTION(BlueprintCallable)
	//void AttackMeleeOK();


	// blabla
	UPROPERTY(EditAnywhere, Category = "Custom|Explosion")
	class UNiagaraSystem* ExplosionParticule;
	
	/**
	 * Radius de l'explosion.
	 */
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (DisplayName = "Radius d'explosion", ClampMin = 0), Category = "Custom|Explosion")
	//float ExploRadius;

	/**
	 * valeur de damage appliquer au player si in range.
	 * Default one 20% of the player max health.
	 */
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Damage de l'explosion", ClampMin = 0), Category = "Custom|Explosion")
	float ExploDmg;

	
};
