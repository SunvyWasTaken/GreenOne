// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_GetHighPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTT_GetHighPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AActor* ActorRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetPlayer.SelectedKeyName)))
	{
		const FVector TargetLoc = ActorRef->GetActorLocation() + UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ActorRef->GetActorUpVector(), RadiusLocation) * TargetHeight;
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocation.SelectedKeyName, TargetLoc);
	}
	return EBTNodeResult::Succeeded;
}
