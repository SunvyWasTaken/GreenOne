// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAtTarget.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_LookAtTarget::UBTT_LookAtTarget()
{
	NodeName = "Look At Target";
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_LookAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	PlayerRefActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerRef.SelectedKeyName));
	if (!PlayerRefActor)
	{
		UE_LOG(LogTemp,Warning, TEXT("Le cast du player ref à fail go check si la reference dans le LookAtTarget et bonne."));
		return EBTNodeResult::Failed;
	}
	if (bUseOld)
	{
		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), PlayerRefActor->GetActorLocation());
		OwnerComp.GetAIOwner()->GetPawn()->SetActorRotation(TargetRotation);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UBTT_LookAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), PlayerRefActor->GetActorLocation());
	const FRotator CurrentRotation = UKismetMathLibrary::RInterpTo(OwnerComp.GetAIOwner()->GetPawn()->GetActorRotation(), TargetRotation, DeltaSeconds, RotationSpeed);
	OwnerComp.GetAIOwner()->GetPawn()->SetActorRotation(CurrentRotation);
	if (TargetRotation.Equals(CurrentRotation, RotationTolerance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
