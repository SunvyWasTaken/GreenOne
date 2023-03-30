// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShootOnPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBTT_ShootOnPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_ShootOnPlayer();
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
