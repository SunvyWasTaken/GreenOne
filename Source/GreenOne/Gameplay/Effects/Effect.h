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

protected:

	UPROPERTY(EditAnywhere, Category = "Effect", DisplayName = "Active le temps d'effect")
	bool bTimeEffect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (EditCondition="bTimeEffect"))
	float TimeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* NSParticleEffect;
	bool bAlreadySpawn = false;

	void InstantiateParticleToActor(const AActor* Actor);

public:
	UEffect();
	
	virtual void ApplyEffect(AActor* Actor) override;
	virtual void ApplyEffect(AActor* Actor, AActor* Source) override;

	UFUNCTION(BlueprintGetter)
	const float GetTimeEffect();
};
