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

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Distance de dodge", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	// Distance de Dodge autour du player.
	// 0 Le dodge ne parcours aucune distance
	// 1 le Drone se retrouve derrière le player.
	float DodgeDistance = 0.4f;

	/**
	 * Distance à la que le drone doit se repositionner par rapport au player
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
