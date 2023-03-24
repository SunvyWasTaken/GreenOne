// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlyingTo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"


UBTT_FlyingTo::UBTT_FlyingTo()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_FlyingTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ControllerRef = Cast<AController>(OwnerComp.GetOwner());
	if (!ControllerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelfRef Failed"));
		return EBTNodeResult::Failed;
	}
	if (TargetRef.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Aucun target n'a était assigner au flying move to."));
		return EBTNodeResult::Aborted;
	}
	return EBTNodeResult::InProgress;
}

void UBTT_FlyingTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	FVector TargetLocation;
	if (AActor* ActorRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
	{
		TargetLocation = ActorRef->GetActorLocation();
	}
	else
	{
		TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetRef.SelectedKeyName);
	}
	if (ACharacter* AIRef = Cast<ACharacter>(ControllerRef->GetPawn()))
	{
		FVector LocTo = TargetLocation - ControllerRef->GetPawn()->GetActorLocation();	
		if (Zlock)
		{
			LocTo.Z = 0.f;
		}
		LocTo.Normalize();
		AIRef->GetMovementComponent()->AddInputVector(LocTo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task n'est pas un character est du coup fail l'appel pour le move."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
	if (TargetLocation.Equals(ControllerRef->GetPawn()->GetActorLocation(), Acceptance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
