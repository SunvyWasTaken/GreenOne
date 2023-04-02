// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Gameplay/Effects/Effect.h"	
#include "FertilizerBase.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UFertilizerBase : public UEffect
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect(AActor* Actor) override;
};
