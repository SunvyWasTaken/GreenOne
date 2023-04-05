// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlyingTo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FlyingAICharacter.h"


UBTT_FlyingTo::UBTT_FlyingTo()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
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
	if (!bOverrideSpeed)
	{
		return EBTNodeResult::InProgress;
	}
	PawnMovementRef = Cast<UCharacterMovementComponent>(ControllerRef->GetCharacter()->GetMovementComponent());
	if (PawnMovementRef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast fail du Pawn movement dans le flying to."));
		return EBTNodeResult::Failed;
	}
	InitialSpeed = PawnMovementRef->MaxFlySpeed;
	InitialAcceleration = PawnMovementRef->GetMaxAcceleration();
	PawnMovementRef->MaxFlySpeed = FlyingSpeed;
	PawnMovementRef->MaxAcceleration *= AccelerationMultiPly;
	return EBTNodeResult::InProgress;
}

void UBTT_FlyingTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AFlyingAICharacter* AIRef = Cast<AFlyingAICharacter>(ControllerRef->GetPawn()))
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
		FVector NormTargetLocation = TargetLocation;
		if (Zlock)
		{
			LocTo.Z = 0.f;
			NormTargetLocation.Z = AIRef->GetActorLocation().Z;
		}
		if (bUseOld)
		{
			AIRef->GetMovementComponent()->AddInputVector(LocTo);
		}
		else
		{
			// To give the rotation axis.
			FVector Direction = LocTo - AIRef->GetActorLocation();
			Direction.Normalize();
			FVector2D TargetInputRotation;
			TargetInputRotation.X = UKismetMathLibrary::DotProduct2D(FVector2D(AIRef->GetActorForwardVector().X, AIRef->GetActorForwardVector().Y), FVector2D(Direction.X, Direction.Y));
			TargetInputRotation.Y = 1.f;
			AIRef->SetRotationAxis(TargetInputRotation);

			// Rotate the player. and move forward
			const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(AIRef->GetActorRotation(), UKismetMathLibrary::MakeRotFromX(LocTo), DeltaSeconds, RotationSpeed);
			AIRef->SetActorRotation(CurrentTargetRotation);
			AIRef->GetMovementComponent()->AddInputVector(AIRef->GetActorForwardVector());
		}			
		if (FVector::Distance(AIRef->GetActorLocation(), NormTargetLocation) <= Acceptance)
		{
			AIRef->SetRotationAxis(FVector2D(0,0));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task n'est pas un character est du coup fail l'appel pour le move."));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
}

void UBTT_FlyingTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (!bOverrideSpeed)
	{
		return;
	}
	if (PawnMovementRef != nullptr)
	{
		PawnMovementRef->MaxFlySpeed = InitialSpeed;
		PawnMovementRef->MaxAcceleration = InitialAcceleration;
		return;
	}
}
