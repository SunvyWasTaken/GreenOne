// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackMelee.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UAttackMelee : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackMelee();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Fight")
	void Attack();

private:

	/** Force appliquée pour repousser le(s) actor(s) */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Impulse", DisplayName = "Force d'impulsion")
	float ImpulseForce = 0.f;
	float ImpulseForceTemp;
	
	/** Valeur de décrémentation pour la force de repouse */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Impulse", DisplayName = "Reduction force d'impulsion")
	float ImpulseForceReduce = 100.f;

	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Damage")
	float Damage = 5.f;

	/** Valeur de cooldown minimum avant d'effectuer de nouveau l'attaque */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|CoolDown", DisplayName = "Valeur du coolDown")
	float MinCoolDown;
	
	float CoolDown;
	float CoolDownTimer;
	bool bActiveCoolDown = false;
	
	/** Valeur d'incrémentation du cooldown après chaque attaque */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|CoolDown", DisplayName = "Valeur d'incrémentation du coolDown")
	float CoolDownIncrease = 2.f;

	UPROPERTY(EditAnywhere, Category = "Fight|Melee|CoolDown", DisplayName = "Activer la réinitialisation du coolDown")
	bool bDelayToResetCoolDown = true;
	/** Valeur du delai pour la réinitialisation du cooldown */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|CoolDown", meta = (EditCondition="bDelayToResetCoolDown"), DisplayName = "Delay reset coolDown")
	float MaxDelayToResetCoolDown;
	
	float DelayToResetCoolDown;
	float DelayToResetCoolDownTimer;
	bool bActiveDelayToResetCoolDown = false;

	/** Offset de la detection selon la position du parent */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Detection", DisplayName = "Offset de la zone de détection")
	float DetectionOffset = 50.f;

	/** Radius de la zone de detection */
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Detection", DisplayName = "Radius de la zone de détection")
	float DetectionRadius = 80.f;

	void DetectActors();
	void ApplyImpulseForce(TArray<FHitResult>& ActorsHit);
	void SetCoolDown(float DeltaTime);
	void SetDelayToResetCoolDown(float DeltaTime);
	void ResetCoolDownValues();
};
