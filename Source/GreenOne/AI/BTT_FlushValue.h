// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FlushValue.generated.h"

/**
 * Met à null la variable.
 * Ne fonctionne que avec les actors.
 */
UCLASS()
class GREENONE_API UBTT_FlushValue : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_FlushValue();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetVariable;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
