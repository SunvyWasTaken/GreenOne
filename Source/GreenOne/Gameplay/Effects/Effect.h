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
public:
	virtual void ApplyEffect(AActor* Actor) override;
};
