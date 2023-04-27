// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AICombo.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBTT_AICombo : public UBTTaskNode
{
	GENERATED_BODY()


public:

	UBTT_AICombo();

#pragma region Task
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	//void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
#pragma endregion	






private:

#pragma region Fight
	int FightStatus;
	//void SetFight(UBehaviorTreeComponent& OwnerComp);
	void Check(UBehaviorTreeComponent& OwnerComp);
	void SetMoveFight(UBehaviorTreeComponent& OwnerComp);
#pragma endregion	













	
};


