// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EffectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEffectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GREENONE_API IEffectInterface
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect(AActor* Actor) {};
};
