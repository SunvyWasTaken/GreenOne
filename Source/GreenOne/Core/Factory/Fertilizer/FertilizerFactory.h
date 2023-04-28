// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerBase.h"

UENUM(BlueprintType)
enum class FertilizerType : uint8
{
	None UMETA(Hidden),
	SlowDown UMETA(DisplayName = "Ralentissement"),
	AttackBonus UMETA(DisplayName = "Bonus d'attaque"),
	Other UMETA(DisplayName = "Test Other"),
	Size UMETA(Hidden),
};

/**
 * 
 */
class GREENONE_API FertilizerFactory
{
public:
	FertilizerFactory();
	~FertilizerFactory();

	static UFertilizerBase* Factory(UObject* Context, FertilizerType Type, TSubclassOf<UFertilizerBase> FertilizerBase);

private:
	template <typename T>
	static UFertilizerBase* InitFertilizer(UObject* Context, TSubclassOf<UFertilizerBase> FertilizerBase);
};


template <typename T>
UFertilizerBase* FertilizerFactory::InitFertilizer(UObject* Context, TSubclassOf<UFertilizerBase> FertilizerBase)
{
	UFertilizerBase* Fertilizer = NewObject<T>(Context,FertilizerBase, NAME_None, RF_NoFlags,
	                                           FertilizerBase->GetDefaultObject());
	return Fertilizer;
}
