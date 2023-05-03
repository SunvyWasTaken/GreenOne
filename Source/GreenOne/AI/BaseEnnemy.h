// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "GreenOne/Gameplay/Effects/ActorEffectInterface.h"
#include "BaseEnnemy.generated.h"

UCLASS()
class GREENONE_API ABaseEnnemy : public ACharacter, public IEntityGame, public IActorEffectInterface
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, float, NbrDamage);

public:
	// Sets default values for this character's properties
	ABaseEnnemy();

	UPROPERTY(EditDefaultsOnly)
	class UAC_DisplayDamage* DamageComp;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Property")
	float Health = 100.f;

	/**
	 * Damage general sur la quel le ratio sera calculer.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Custom|Property")
	float Damage = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Property", DisplayName = "Speed Max", meta = (ForceUnits = "cm/s"));
	float MaxSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Property", DisplayName = "Number of Hit");
	float NumberOfHit = 0.f;

	/**
	 * Percentage de reduction damage when it's a ally that hit.
	 */
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Damage reduction from ally", ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1), Category = "Custom|Property")
	float PercentDamageReduct = 0.5;

	/**
	 * Return une valeur entre 0 et 1 correspondant au percentage de vie de l'entity
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	float GetPercentHealth();

	UPROPERTY(BlueprintAssignable, Category = "Custom|Damage")
	FOnTakeDamage OnTakeDamage;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Custom|Damage")
	void EntityTakeDamage(float DamageApply, FName BoneNameHit, AActor* DamageSource = nullptr);

#pragma region Effect
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EntityTakeEffect(UEffect* Effect, AActor* Source = nullptr);
	UFUNCTION(BlueprintCallable)
	virtual void ResetEffect(UEffect* Effect, const float DelayToReset);
	void ResetParticleEffect(const UNiagaraSystem* Particle) const;
	void ResetMaterialEffect() const;
	void ResetAllParticle() const;
#pragma endregion 
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerRef(AActor* ref);

	class AEnnemySpawner* SpawnerRef;

	virtual void UpdateMaxSpeed(float Speed) {};

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Property")
	bool IsFriendlyFire = true;

protected:

	/************************************************************************/
	/* Draw Debug Life bar													*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, Category = "Debug|Life")
	class UWidgetComponent* LifeBarComponent;

	UPROPERTY(EditAnywhere, Category = "Debug|Life")
	bool DrawLifeBar = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = DrawLifeBar), Category = "Debug|Life")
	TSubclassOf<UUserWidget> LifeBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Combat")
	TArray<FName> ListWeakPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Combat")
	float CritMultiplyer = 2.f;

	/**
	 * List de différent palier de vie.
	 * En lien avec la liste de matérial. lors que la vie de l'index 0 sera atteint.
	 * Le material dans la list MatTreshold à l'index 0 sera appliquer au mesh.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Combat")
	TArray<int32> LifeTreshold;

	/**
	 * List des mat qui seront appliquer à la mesh lors des différents palier de vie
	 * La liste doit être de la même longueur que celle de Lifetreshold sinon EXPLOSION!!!
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Combat")
	TArray<UMaterialInterface*> MatTreshold;

	FTimerHandle TimeToResetEffect;

private:

	float MaxHealth = 0;

	void ChangeTextureBaseHealth();

protected:

	virtual void DeadEntity();
	void DestroyActor();

};