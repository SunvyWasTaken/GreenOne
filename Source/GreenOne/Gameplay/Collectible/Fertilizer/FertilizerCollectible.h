// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Core/Struct/Fertilizer/FFertilizerTankStruct.h"
#include "GreenOne/Gameplay/Collectible/CollectibleActorBase.h"
#include "FertilizerCollectible.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AFertilizerCollectible : public ACollectibleActorBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer")
	FFertilizerTankStruct CollectibleFertilizerTankStruct;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	AFertilizerCollectible();

	virtual void Action(AActor* Collector) override;
};
