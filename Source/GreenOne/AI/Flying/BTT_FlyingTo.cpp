// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlyingTo.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FlyingAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// TODO Regarder le Dot product parcequ'il est tjr pencher très legerement.
// TODO Faire un trace vers l'avant pour voir si il va collide un truc après le trace vers le player.
UBTT_FlyingTo::UBTT_FlyingTo()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

//This function is part of the UBTT_FlyingTo class, which is a subclass of the UBTTaskNode class. It is used to execute a task in a behavior tree. It first checks if
// the controller reference is valid, and if not, it returns a failed result. It then checks if a target has been assigned, and if not, it returns an aborted result
//. It then checks if the override speed is set, and if not, it returns an in progress result. It then casts the character movement component and if it fails, it returns
// a failed result. Finally, it sets the initial speed and acceleration and returns an in progress result.
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
		MyCapsuleRadius = AIRef->GetCapsuleComponent()->GetScaledCapsuleRadius();
		MyCapsuleHeight = AIRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		if (AActor* ActorRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
		{
			TargetLocation = ActorRef->GetActorLocation();
		}
		else
		{
			TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetRef.SelectedKeyName);
		}

		TickCheckCollision(DeltaSeconds, AIRef, OwnerComp);

		FVector LocTo = TargetLocation - AIRef->GetActorLocation();

		if (Zlock)
		{
			LocTo.Z = 0.f;
			TargetLocation.Z = AIRef->GetActorLocation().Z;
		}


		TickAddInputToPawn(DeltaSeconds, AIRef, LocTo);

		if (FVector::Distance(AIRef->GetActorLocation(), TargetLocation) <= Acceptance)
		{
			AIRef->SetRotationAxis(FVector2D(0, 0));
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

void UBTT_FlyingTo::TickAddInputToPawn(float Deltatime, AFlyingAICharacter* BirdRef, FVector TargetLoc)
{
	if (bUseOld)
	{
		BirdRef->GetMovementComponent()->AddInputVector(TargetLoc);
	}
	else
	{
		// To give the rotation axis.
		FVector Direction = TargetLoc - BirdRef->GetActorLocation();
		Direction.Normalize();
		FVector2D TargetInputRotation;
		TargetInputRotation.X = UKismetMathLibrary::Dot_VectorVector(BirdRef->GetActorForwardVector(), Direction);
		TargetInputRotation.Y = 1.f;
		BirdRef->SetRotationAxis(TargetInputRotation);

		// Rotate the player. and move forward
		const FRotator CurrentTargetRotation = UKismetMathLibrary::RInterpTo(BirdRef->GetActorRotation(), UKismetMathLibrary::MakeRotFromX(TargetLoc), Deltatime, RotationSpeed);
		BirdRef->SetActorRotation(CurrentTargetRotation);
		BirdRef->GetMovementComponent()->AddInputVector(BirdRef->GetActorForwardVector());
	}
}

void UBTT_FlyingTo::TickCheckCollision(float Deltatime, AFlyingAICharacter* BirdRef, UBehaviorTreeComponent& OwnerComp)
{
	if (!bUseAvoidance)
	{ return; }

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(BirdRef);
	FHitResult Outhit;

	FVector Direction = TargetLocation - BirdRef->GetActorLocation();
	const float LenghtTrace = UKismetMathLibrary::Clamp(Direction.Length(), 0.f, DistanceWallDetection);

	Direction.Normalize();
	FVector LocTo = BirdRef->GetActorLocation() + (TraceForward ? (BirdRef->GetActorForwardVector()) : (Direction)) * LenghtTrace;

	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), BirdRef->GetActorLocation(), LocTo, MyCapsuleRadius, MyCapsuleHeight, UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::ForOneFrame, Outhit, true);

	if (!Outhit.bBlockingHit)
	{ return; }


	TargetLocation = BirdRef->GetActorLocation() + (UKismetMathLibrary::ProjectVectorOnToPlane(Direction, Outhit.ImpactNormal)* DistanceWallDetection);

	//UKismetSystemLibrary::DrawDebugArrow(GetWorld(), BirdRef->GetActorLocation(), TargetLocation, 5.f, FLinearColor::Red, 0.2f, 5.f);

	return;
}
