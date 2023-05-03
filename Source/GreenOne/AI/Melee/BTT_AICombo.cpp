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
	FightMStatus = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	SetMoveFight(OwnerComp);
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [&]()
	{
		//OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
		{
			PlayerRef->CanM_Fighting = false;
			PlayerRef->CanMR_Fighting = false;
			HitCheck(OwnerComp);
		}
		UE_LOG(LogTemp, Warning, TEXT("timer"));
	},1.5f, false);
	return EBTNodeResult::InProgress;
}

void UBTT_AICombo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("taskfinish"));
		FightStatus = 0;
		PlayerRef->CanCombo = false;
		PlayerRef->CanM_Fighting = false;
		PlayerRef->CanMR_Fighting = false;
		PlayerRef->Can_Fighting = false;
		PlayerRef->CanR_Fighting = false;
		PlayerRef->StopMouv();
	}
}


void UBTT_AICombo::SetMoveFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		PlayerRef->CanM_Fighting = true;
		if(FightMStatus == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouv gauche"));
			FightStatus = -1;
		}
		else if(FightMStatus == 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouv droite"));
			PlayerRef->CanMR_Fighting = true;
			FightStatus = 1;
		}
		Check(OwnerComp);
	}
}


void UBTT_AICombo::SetFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("setfight"));
		PlayerRef->Can_Fighting = true;
		if (FightMStatus == -1)
			{
				PlayerRef->CanR_Fighting = true;
				UE_LOG(LogTemp, Warning, TEXT("id_fightR"));
			}
		if (FightMStatus == 1)
			{
				
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
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(	PlayerRef->CanM_Fighting == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouvL faux"));
		}
		if(	PlayerRef->CanMR_Fighting == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouvR faux"));
		}
	}
	
}

void UBTT_AICombo::HitCheck(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(	PlayerRef->CanCombo == true)
		{
			PlayerRef->StopMouv();
			UE_LOG(LogTemp, Warning, TEXT("Can Combo"));
			SetFight(OwnerComp);
		}
		else{UE_LOG(LogTemp, Warning, TEXT("cest la merde"))};
	}
}

void UBTT_AICombo::Reset(UBehaviorTreeComponent& OwnerComp)
{

	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Reset Value Mouv"));
		PlayerRef->CanM_Fighting = false;
		PlayerRef->CanMR_Fighting = false;
		PlayerRef->StopMouv();
		HitCheck(OwnerComp);
	}
}
