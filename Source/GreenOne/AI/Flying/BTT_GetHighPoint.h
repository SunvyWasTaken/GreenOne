// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_GetHighPoint.generated.h"

/**
 * Get a high point above the player.
 */
UCLASS()
class GREENONE_API UBTT_GetHighPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetPlayer;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetLocation;

	/**
	 * Angle d'aleatoire autour du player
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName  = "Angle aleatoire", UIMin = 0, UIMax = 180.f, ClampMin = 0.f, ClampMax = 180.f))
	float RadiusLocation = 60.f;

	/**
	 * Hauteur au dessus du player
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Height au dessus du player", ClampMin = 10.f))
	float TargetHeight = 600.f;

};
