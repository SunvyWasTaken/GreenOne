// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FlyingTo.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class GREENONE_API UBTT_FlyingTo : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_FlyingTo();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	/// <summary>
	/// This property defines the Acceptance Radius with a minimum value of 0.
	/// </summary>
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Acceptance Radius", ClampMin = 0))
		double Acceptance = 150.f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Target move to"))
		FBlackboardKeySelector TargetRef;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float RotationSpeed = 2.f;

	/**
	 * If true the AI would fly without moving in the z axis.
	 */
	UPROPERTY(EditAnywhere)
		bool Zlock = false;

	/**
	* Use the Old flying system
	*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, meta = (DisplayName = "Use Old System"))
		bool bUseOld = true;

private:

	AController* ControllerRef;

	FVector TargetLocation;

};
