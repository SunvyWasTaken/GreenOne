// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlushValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FlushValue::UBTT_FlushValue()
{
	// Allowed UObject
	TargetVariable.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FlushValue, TargetVariable), UObject::StaticClass());
}

EBTNodeResult::Type UBTT_FlushValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetVariable.SelectedKeyName, nullptr);
	return EBTNodeResult::Succeeded;
}

