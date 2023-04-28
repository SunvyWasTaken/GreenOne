// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsAtHealthPercent.h"
#include "BaseEnnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

bool UBTD_IsAtHealthPercent::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (ABaseEnnemy* PawnRef = Cast<ABaseEnnemy>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		return PawnRef->GetPercentHealth() <= HealthPercent;
	}
	UE_LOG(LogTemp, Warning, TEXT("The current task owner or the sefl ref in the BTD_IsAtHealthPercent is incorrect."));
	return false;
}