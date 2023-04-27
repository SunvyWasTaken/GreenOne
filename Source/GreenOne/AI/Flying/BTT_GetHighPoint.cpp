// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_GetHighPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


EBTNodeResult::Type UBTT_GetHighPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AActor* ActorRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetPlayer.SelectedKeyName)))
	{
		FVector UpVec = ActorRef->GetActorUpVector();
		UpVec.RotateAngleAxis(RadiusLocation/2, ActorRef->GetActorForwardVector());
		const FVector TargetLoc = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation() + UKismetMathLibrary::RandomUnitVectorInConeInDegrees(UpVec, RadiusLocation) * TargetHeight;
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocation.SelectedKeyName, TargetLoc);
	}
	return EBTNodeResult::Succeeded;
}
