// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsTargetAttacking.h"
#include "GreenOne/GreenOneCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsTargetAttacking::UBTD_IsTargetAttacking()
{
	bCreateNodeInstance = true;
}

bool UBTD_IsTargetAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (AGreenOneCharacter* PlayerRef = Cast<AGreenOneCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
	{
		return PlayerRef->IsAttacking();
	}
	UE_LOG(LogTemp, Warning, TEXT("Fail Dans le BTD_IsTargetAttacking la variable TargetRef n'est pas deriver de AGreenCharacter"));
	return false;
}
