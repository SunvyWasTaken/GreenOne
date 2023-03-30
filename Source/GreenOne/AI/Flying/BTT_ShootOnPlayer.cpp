// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ShootOnPlayer.h"
#include "FlyingAiCharacter.h"
#include "AIController.h"

UBTT_ShootOnPlayer::UBTT_ShootOnPlayer()
{
	bCreateNodeInstance = true;
}

//This function is used to execute the task ShootOnPlayer.
//It checks if the owner of the task is a child of AFlyingAICharacter.
//If it is, it calls the Shoot() function and returns a Succeeded result. Otherwise, it logs a warning and returns a Failed result. 
EBTNodeResult::Type UBTT_ShootOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Check if the owner of the task is a child of AFlyingAICharacter
	if (AFlyingAICharacter* PawnRef = Cast<AFlyingAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		//Call the Shoot() function
		PawnRef->Shoot();
		//Return a Succeeded result
		return EBTNodeResult::Succeeded;
	}
	//Log a warning
	UE_LOG(LogTemp, Warning, TEXT("Le owner du task ShootOnPlayer n'est pas a child de AFlyingAICharacter."));
	//Return a Failed result
	return EBTNodeResult::Failed;
}
