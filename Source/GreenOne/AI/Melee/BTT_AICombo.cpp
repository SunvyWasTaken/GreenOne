// Fill out your copyright notice in the Description page of Project Settings.


#include "GreenOne/AI/Melee/BTT_AICombo.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GreenOne/AI/Melee/MeleeAICharacter.h"

UBTT_AICombo::UBTT_AICombo()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_AICombo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(IsValid(&OwnerComp))
	{
		// Log
		UE_LOG(LogTemp, Warning, TEXT("BTT_AICombo::ExecuteTask"));
		return EBTNodeResult::Failed;
	}
	
	FightMStatus = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	SetMoveFight(OwnerComp);
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [&]()
	{
		//OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
		UE_LOG(LogTemp, Warning, TEXT("IA_Ref : %s"), *IA_Ref->GetName());
		
		if(IsValid(IA_Ref))
		{
			IA_Ref->CanM_Fighting = false;
			IA_Ref->CanMR_Fighting = false;
			HitCheck(OwnerComp);
		}

	},1.5f, false);
	return EBTNodeResult::InProgress;
}

void UBTT_AICombo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FightStatus = 0;
		IA_Ref->CanCombo = false;
		IA_Ref->CanM_Fighting = false;
		IA_Ref->CanMR_Fighting = false;
		IA_Ref->Can_Fighting = false;
		IA_Ref->CanR_Fighting = false;
		IA_Ref->StopMov();
	}
}

void UBTT_AICombo::SetMoveFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		IA_Ref->CanM_Fighting = true;
		if(FightMStatus == -1)
		{
			FightStatus = -1;
		}
		else if(FightMStatus == 1)
		{
			IA_Ref->CanMR_Fighting = true;
			FightStatus = 1;
		}
		Check(OwnerComp);
	}
}

void UBTT_AICombo::SetFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		IA_Ref->Can_Fighting = true;
		if (FightMStatus == -1)
		{
			IA_Ref->CanR_Fighting = true;
		}
		if (FightMStatus == 1)
		{
			// LOG DE MERDE
			UE_LOG(LogTemp, Warning, TEXT("id_fightL"));
		}
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, [&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		},2.0f, false);
	}
}

void UBTT_AICombo::Check(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(	IA_Ref->CanM_Fighting == false)
		{
			// Log de merde
			UE_LOG(LogTemp, Warning, TEXT("mouvL faux"));
		}
		if(	IA_Ref->CanMR_Fighting == false)
		{
			// Log de merde
			UE_LOG(LogTemp, Warning, TEXT("mouvR faux"));
		}
	}
	
}

void UBTT_AICombo::HitCheck(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(	IA_Ref->CanCombo == true)
		{
			IA_Ref->StopMov();
			SetFight(OwnerComp);
		}
	}
}

void UBTT_AICombo::Reset(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* IA_Ref = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		IA_Ref->CanM_Fighting = false;
		IA_Ref->CanMR_Fighting = false;
		IA_Ref->StopMov();
		HitCheck(OwnerComp);
	}
}
