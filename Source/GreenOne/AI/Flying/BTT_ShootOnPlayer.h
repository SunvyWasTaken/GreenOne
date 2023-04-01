// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShootOnPlayer.generated.h"

UENUM()
enum class EBehavior : uint8
{
	None,
	Wait,
	Faild,
	Skip
};

// TODO Add the feature that AI can try to predict where the player will be based on the speed of the bullet.

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

private:

	class AFlyingAICharacter* PawnRef;

public:

	UPROPERTY(EditAnywhere)
		EBehavior CurrentBehavior;

};
