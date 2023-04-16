// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsAtHealthPercent.generated.h"

/**
 * Check if the task owner is at or under the given health percent
 */
UCLASS()
class GREENONE_API UBTD_IsAtHealthPercent : public UBTDecorator
{
	GENERATED_BODY()

public:

	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	UPROPERTY(EditAnywhere, meta = (UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	float HealthPercent;

};
