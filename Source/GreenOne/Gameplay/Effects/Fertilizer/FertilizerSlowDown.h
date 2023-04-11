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

	UPROPERTY(EditAnywhere, Transient, Category = "Effect", DisplayName = "Max de ralentissement", meta = (ClampMin = 0, ClampMax = 100, ForceUnits = "%"))
	int32 MaxPercentSlownDown = 50;

	UPROPERTY(EditAnywhere, Category = "Effect", DisplayName = "% de ralentissement", meta = (ForceUnits = "%"))
	int32 PercentSlowDown = 10;

public:
	UFertilizerSlowDown();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	/**
	 * Applique l'effet sur l'acteur
	 */
	virtual void ApplyEffect(AActor* Actor) override;

	/**
	 * Retourne le total de pourcentage affecté à l'acteur 
	 */
	float GetTotalPercent(float Value, float ReduceValue);
};
