// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerBase.h"
#include "MyFertilizerAttackBonus.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UMyFertilizerAttackBonus : public UFertilizerBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Effect", DisplayName = "Bonus maximal d'attaque", meta = (ClampMin = 0, ClampMax = 100, ForceUnits = "%"))
	int32 MaxPercentAtkBonus = 200;

	UPROPERTY(EditAnywhere, Category = "Effect", DisplayName = "% d'attaque bonus", meta = (ForceUnits = "%"))
	int32 PercentAtkBonus = 20;

public:
	UMyFertilizerAttackBonus();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	/**
	 * Applique l'effet sur l'acteur
	 */
	virtual void ApplyEffect(AActor* Actor, AActor* Source) override;
};
