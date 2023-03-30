// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_QuickMoveLOrR.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_QuickMoveLOrR::UBTT_QuickMoveLOrR()
{
	bNotifyTick = true;
	MoveTime = 1.f;
	Speed = 600.f;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_QuickMoveLOrR::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CurrentTime = MoveTime;
	// Optimized and secured

	if (APawn* SelfPawnRef = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(SelfPawnRef->GetMovementComponent()))
		{
			MovementComp->bOrientRotationToMovement = false;
		}
	}
	DirectionValue = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	return EBTNodeResult::InProgress;
}

void UBTT_QuickMoveLOrR::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	CurrentTime -= DeltaSeconds;

	if (APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		const FVector TargetDirection = Pawn->GetActorRightVector() * DirectionValue;
		Pawn->GetMovementComponent()->AddInputVector(TargetDirection);
		if (const AActor* PlayerRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
		{
			Pawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), PlayerRef->GetActorLocation()));
		}
	}
	if (CurrentTime <= 0.f)
	{
		if (APawn* SelfPawnRef = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
		{
			if (UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(SelfPawnRef->GetMovementComponent()))
			{
				MovementComp->bOrientRotationToMovement = true;
			}
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

