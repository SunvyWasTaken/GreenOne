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
	FightStatus = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	SetMoveFight(OwnerComp);
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [&]()
	{
		//OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		UE_LOG(LogTemp, Warning, TEXT("timer"));
	},3.0f, false);
	return EBTNodeResult::InProgress;
}

void UBTT_AICombo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		PlayerRef->CanM_Fighting = false;
		PlayerRef->CanMR_Fighting = false;
	}
}


void UBTT_AICombo::SetMoveFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		PlayerRef->CanM_Fighting = true;
		if(FightStatus == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouv gauche"));
			//PlayerRef->CanML_Fighting = true;
			//PlayerRef->CanMR_Fighting = false; 
		}
		else if(FightStatus == 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouv droite"));
			PlayerRef->CanMR_Fighting = true;
			//PlayerRef->CanML_Fighting = false; 
		}
		Check(OwnerComp);
	}
}
																				

void UBTT_AICombo::Check(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(	PlayerRef->CanM_Fighting == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouv faux"));
		}
		if(	PlayerRef->CanMR_Fighting == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("mouvR faux"));
		}
	}
	
}

