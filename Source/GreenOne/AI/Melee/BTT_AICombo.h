// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeAICharacter.h"
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

	// ** FUNCTIONS ** //
	
	//void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);

	// ** ATTRIBUTES ** //
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
#pragma endregion	

	// FORCEINLINE AMeleeAICharacter* GetMeleeAICharacter() const { return IA_Ref; }
	
private:

	// AMeleeAICharacter* IA_Ref = nullptr;

#pragma region Fight

	// ** FUNCTIONS ** //
	void SetFight(UBehaviorTreeComponent& OwnerComp);
	void SetMoveFight(UBehaviorTreeComponent& OwnerComp);
	void Check(UBehaviorTreeComponent& OwnerComp);
	void HitCheck(UBehaviorTreeComponent& OwnerComp);
	void Reset(UBehaviorTreeComponent& OwnerComp);

	// ** ATTRIBUTES ** //
	int FightMStatus;
	int FightStatus = 0;
	
#pragma endregion	













	
};


