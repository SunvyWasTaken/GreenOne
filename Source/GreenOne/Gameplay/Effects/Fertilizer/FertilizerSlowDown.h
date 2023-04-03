// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FertilizerBase.h"
#include "FertilizerSlowDown.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UFertilizerSlowDown : public UFertilizerBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Effect")
	int32 MaxPercentSlownDown = 50;

	UPROPERTY(EditAnywhere, Category = "Effect")
	int32 PercentSlowDown = 10;

	int32 CalculPercentSlowDown = 0;
	
public:
	UFertilizerSlowDown();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void ApplyEffect(AActor* Actor) override;

	float GetTotalPercent(float Value, float ReduceValue);
};
