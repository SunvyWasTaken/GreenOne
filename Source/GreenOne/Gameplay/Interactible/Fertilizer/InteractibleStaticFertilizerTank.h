// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FInteractibleFertilizerTankStruct.h"
#include "GreenOne/Gameplay/Interactible/InteractibleStaticActor.h"
#include "InteractibleStaticFertilizerTank.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleStaticFertilizerTank : public AInteractibleStaticActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer")
	FInteractibleFertilizerTankStruct InteractibleFertilizerTankStruct;

public:
	AInteractibleStaticFertilizerTank();

	virtual void Action(AActor* Interactor) override;
};
