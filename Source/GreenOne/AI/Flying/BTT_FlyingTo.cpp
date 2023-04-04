// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlyingTo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UBTT_FlyingTo::UBTT_FlyingTo()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
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

	if (ACharacter* AIRef = Cast<ACharacter>(ControllerRef->GetPawn()))
	{
		if (AActor* ActorRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
		{
			TargetLocation = ActorRef->GetActorLocation();
		}
		else
		{
			TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetRef.SelectedKeyName);
		}
		FVector LocTo = TargetLocation - AIRef->GetActorLocation();	
		if (Zlock)
		{
			LocTo.Z = 0.f;
		}
		if (bUseOld)
		{
			AIRef->GetMovementComponent()->AddInputVector(LocTo);
		}
		else
		{
			const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(AIRef->GetActorRotation(), UKismetMathLibrary::MakeRotFromX(LocTo), DeltaSeconds, RotationSpeed);
			AIRef->SetActorRotation(CurrentTargetRotation);
			AIRef->GetMovementComponent()->AddInputVector(AIRef->GetActorForwardVector());
		}
		FVector NormTargetLocation = TargetLocation;
		if (Zlock)
		{
			NormTargetLocation.Z = AIRef->GetActorLocation().Z;
		}
		if (FVector::Distance(AIRef->GetActorLocation(), NormTargetLocation) <= Acceptance)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task n'est pas un character est du coup fail l'appel pour le move."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
}
