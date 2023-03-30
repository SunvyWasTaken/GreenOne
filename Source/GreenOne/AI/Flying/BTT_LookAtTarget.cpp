// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAtTarget.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_LookAtTarget::UBTT_LookAtTarget()
{
	NodeName = "Look At Target";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_LookAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* PlayerRefActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerRef.SelectedKeyName));
	if (!PlayerRefActor)
	{
		UE_LOG(LogTemp,Warning, TEXT("Le cast du player ref à fail go check si la reference dans le LookAtTarget et bonne."));
		return EBTNodeResult::Failed;
	}
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), PlayerRefActor->GetActorLocation());
	OwnerComp.GetAIOwner()->GetPawn()->SetActorRotation(TargetRotation);
	return EBTNodeResult::Succeeded;
}
