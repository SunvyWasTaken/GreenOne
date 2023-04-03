// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FertilizerBase.h"
#include "FertilizerOther.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UFertilizerOther : public UFertilizerBase
{
	GENERATED_BODY()
public:
	UFertilizerOther();
	virtual void ApplyEffect(AActor* Actor) override;
	
};
