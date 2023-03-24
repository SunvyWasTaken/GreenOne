// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_LookAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBTT_LookAtTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_LookAtTarget();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerRef;
	
};
