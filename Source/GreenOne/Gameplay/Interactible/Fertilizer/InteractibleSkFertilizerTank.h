// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Core/Struct/Fertilizer/FFertilizerTankStruct.h"
#include "GreenOne/Gameplay/Interactible/InteractibleSkeletonActor.h"
#include "InteractibleSkFertilizerTank.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleSkFertilizerTank : public AInteractibleSkeletonActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer")
	FFertilizerTankStruct InteractibleFertilizerTankStruct;

public:
	AInteractibleSkFertilizerTank();

	virtual void Action(AActor* Interactor) override;
};
