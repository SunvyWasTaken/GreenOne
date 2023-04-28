// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Gameplay/Interactible/InteractibleBaseActor.h"
#include "InteractibleFertilizerTank.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FInteractibleFertilizerTankStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", meta = (ClampMin = 0, ClampMax = 100), DisplayName = "Pourcentage d'engrais")
	float FertilizerValueToGive = 100.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", DisplayName = "Type d'engrais")
	FertilizerType FertilizerType;
};

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleFertilizerTank : public AInteractibleBaseActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer")
	FInteractibleFertilizerTankStruct InteractibleFertilizerTankStruct;
	
public:
	AInteractibleFertilizerTank();

	virtual void Action(AActor* Interactor) override;
};
