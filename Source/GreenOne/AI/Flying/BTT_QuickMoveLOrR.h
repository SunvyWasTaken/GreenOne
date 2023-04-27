// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_QuickMoveLOrR.generated.h"

UENUM()
enum class EDimension : uint8
{
	Horizontal,
	Vertical,
	Diagonal,
	Alternate
};

// TODO ajouter un dot pour faire en sorte que le truc aille pas non plus au dessus du player Ou en dessous totalement de la map.

/**
 *
 */
UCLASS()
class GREENONE_API UBTT_QuickMoveLOrR : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_QuickMoveLOrR();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

public:

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector TargetRef;

	/// <summary>
	/// Override the Speed value. 
	/// This value is not currently being used. 
	/// Awaiting approval.
	/// </summary>
	UPROPERTY(EditAnywhere)
	EDimension Deplacement;

	/// <summary>
	/// Time taken to move from one point to another
	/// </summary>
	UPROPERTY(EditAnywhere)
	float MoveTime;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Pourcentage d'alignement", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	float PercentAlignment;

private:

	float CurrentTime;

	int DirectionValue;

	int OtherDirection;

	bool IsHorizontal;

	void SetFlyingRotation(APawn* RefOwner, FVector2D Axis);

};
