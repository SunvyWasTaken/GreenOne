// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Dodge.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GREENONE_API UBTT_Dodge : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_Dodge();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector TargetRef;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Dodge Distance", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
		// Dodge distance around the player.
		// 0 Dodge does not travel any distance
		// 1 Drone is behind the player.
		float DodgeDistance = 0.4f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Should Get to safe distance to"))
	bool bShouldGetSafeDistance = false;

	/**
	 * Distance at which the drone should reposition itself relative to the player
	 */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float SafeDistance = 500.f;

private:

	AController* ControllerRef;

	AActor* PlayerRef;

	float DodgeDirection = 1.f;

	float FlyingSpeedInit = 0.f;

	class UCharacterMovementComponent* CharaMovementComp;
};
