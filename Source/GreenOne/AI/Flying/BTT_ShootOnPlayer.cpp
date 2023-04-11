// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ShootOnPlayer.h"
#include "FlyingAiCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"

UBTT_ShootOnPlayer::UBTT_ShootOnPlayer()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	CurrentBehavior = EBehavior::Skip;
}

//This function is used to execute the task ShootOnPlayer.
//It checks if the owner of the task is a child of AFlyingAICharacter.
//If it is, it calls the Shoot() function and returns a Succeeded result. Otherwise, it logs a warning and returns a Failed result. 
EBTNodeResult::Type UBTT_ShootOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	PawnRef = Cast<AFlyingAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (PawnRef != nullptr)
	{
		if (bCheckCanShoot)
		{
			if (CheckTargetVisible())
			{
				switch (ShootBehavior)
				{
				case EBehavior::Skip:
					return EBTNodeResult::Succeeded;
					break;
				case EBehavior::Faild:
					return EBTNodeResult::Failed;
					break;
				}
			}
		}
		if (!PawnRef->CanShoot())
		{
			switch (CurrentBehavior)
			{
			case EBehavior::Wait:
				return EBTNodeResult::InProgress;
				break;
			case EBehavior::Faild:
				return EBTNodeResult::Failed;
				break;
			}
		}
		//Call the Shoot() function
		PawnRef->Shoot();
		//Return a Succeeded result
		return EBTNodeResult::Succeeded;
	}
	//Log a warning
	UE_LOG(LogTemp, Warning, TEXT("Le owner du task ShootOnPlayer n'est pas un child de AFlyingAICharacter."));
	//Return a Failed result
	return EBTNodeResult::Failed;
}

void UBTT_ShootOnPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (PawnRef != nullptr)
	{
		if (PawnRef->CanShoot())
		{
			if (CheckTargetVisible())
			{
				switch (ShootBehavior)
				{
				case EBehavior::Skip:
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return;
					break;
				case EBehavior::Faild:
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return;
					break;
				}
			}
			PawnRef->Shoot();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

bool UBTT_ShootOnPlayer::CheckTargetVisible()
{
	FHitResult Outhit;
	AActor* PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<AActor*> ActorToIgnore;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), PawnRef->GetActorLocation() + (PawnRef->GetActorForwardVector() * (1 + PawnRef->GetCapsuleComponent()->GetScaledCapsuleRadius())), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation(), UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::ForOneFrame, Outhit, true);
	if (Outhit.bBlockingHit)
	{
		if (Outhit.GetActor() == nullptr)
		{
			return false;
		}
		UE_LOG(LogTemp, Warning, TEXT("Hit : %s"),*Outhit.GetActor()->GetFName().ToString());
	}
	return Outhit.bBlockingHit;

	//return GetWorld()->LineTraceSingleByChannel(Outhit, PawnRef->GetActorLocation() + 25.f, PlayerRef->GetActorLocation(), ECC_Visibility);
}
