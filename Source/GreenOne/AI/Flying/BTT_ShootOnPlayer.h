// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShootOnPlayer.generated.h"

/**
 *
 */
UCLASS()
class GREENONE_API UBTT_ShootOnPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_ShootOnPlayer();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	/// <summary>
	/// If the task should wait that the flying AI can shoot
	/// If this condition is true, then the ShouldFail condition will necessarily be false.
	/// If none of the condition are enable the task will attempt to shoot and success.
	/// </summary>
	UPROPERTY(EditAnywhere)
	bool ShouldWait;

	/// <summary>
	/// If true, then the task will fail if the AI flying cannot pull "IsInCooldown".
	/// If the condition shouldWait is true, then the task will never fail.
	/// If none of the condition are enable the task will attempt to shoot and success.
	/// </summary>
	UPROPERTY(EditAnywhere)
	bool ShouldFail;

private:

	class AFlyingAICharacter* PawnRef;

};
