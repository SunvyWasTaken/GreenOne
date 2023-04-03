// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FertilizerSlowDown.h"

UENUM(BlueprintType)
enum class FertilizerType : uint8
{
	SlowDown UMETA(DisplayName = "Ralentissement"),
	Other UMETA(DisplayName = "Test Other"),
};

/**
 * 
 */
class GREENONE_API FertilizerFactory
{
public:
	FertilizerFactory();
	~FertilizerFactory();

	static UFertilizerBase* Factory(FertilizerType Type, TSubclassOf<UFertilizerBase> FertilizerBase);
};
