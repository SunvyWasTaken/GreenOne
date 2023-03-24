// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsTargetAttacking.generated.h"

/**
 * Node use to check if the Target selected in the TargetRef is attacking or not,
 * The Target must be a child of the GreenOneCharacter Otherwise it will fail.
 */
UCLASS()
class GREENONE_API UBTD_IsTargetAttacking : public UBTDecorator
{
	GENERATED_BODY()

public:

	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetRef;
	
};
