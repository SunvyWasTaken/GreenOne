// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectInterface.h"
#include "Effect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GREENONE_API UEffect : public UObject, public IEffectInterface
{
	GENERATED_BODY()

	void InitMaterialEffect();
	
protected:

	UPROPERTY(EditAnywhere, Category = "Effect", DisplayName = "Active le temps d'effect")
	bool bTimeEffect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (EditCondition="bTimeEffect"))
	float TimeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|VFX|Particle", DisplayName = "Particle de l'effet")
	class UNiagaraSystem* NSParticleEffect;
	class UNiagaraComponent* NiagaraComponentTemp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|VFX|Material", DisplayName = "Texture de l'effet")
	UMaterial* MaterialEffect;
	UMaterialInstanceDynamic* DynamicMaterialEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|VFX|Material", DisplayName = "Surchager la couleur de la texture")
	bool bOverrideColor = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|VFX|Material", meta = (EditCondition="bOverrideColor"), DisplayName = "Couleur de la texture")
	FLinearColor ColorMaterialEffect;

	void InstantiateParticleToActor(const AActor* Actor);
	void ApplyMaterialEffect(const AActor* Actor);
	bool IsActorEffectInterface(const AActor* Actor);
	
public:
	UEffect();
	void Init();
	
	virtual void ApplyEffect(AActor* Actor) override;
	virtual void ApplyEffect(AActor* Actor, AActor* Source) override;

	UFUNCTION(BlueprintGetter)
	float GetTimeEffect() const;
	UFUNCTION(BlueprintGetter)
	UNiagaraSystem* GetParticleEffect() const;
	bool IsAlreadyExist(const AActor* Actor);
	bool IsMaterialEffectAlreadyExist(const AActor* Actor) const;
	
	class UNiagaraComponent* GetParticleComponent() const;
};
