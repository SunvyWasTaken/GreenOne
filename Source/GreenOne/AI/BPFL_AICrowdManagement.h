// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "DetourCrowdAIController.h"
#include "Navigation/CrowdFollowingComponent.h" 
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFL_AICrowdManagement.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBPFL_AICrowdManagement : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//*separatio*//
	UFUNCTION(BlueprintCallable, Category = DetourCrowd)
	static void SetSeparationWeight(UCrowdFollowingComponent* inCrowdFolowingComponent, int32 inWeight);	
};
