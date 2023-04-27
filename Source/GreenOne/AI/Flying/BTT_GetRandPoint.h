// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_GetRandPoint.generated.h"

 UENUM()
 enum class EZoneType
 {
	Area,
	Path
 };

 /**
 *	Return a Random point depending on the type of search it can be in a define area around a point or the start location
 * Or it can travel a long a path if it has the depending actor.
 */
UCLASS(BlueprintType)
class GREENONE_API UBTT_GetRandPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_GetRandPoint();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EZoneType SearchType;


	UPROPERTY(EditAnywhere, meta = (DisplayName = "Distance de recherche", ClampMin = 0), Category = "Area")
	float SearchRadArea = 50.f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Distance Alentour des points", ClampMin = 0), Category = "Path")
	float SearchRadPath = 50.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Nom attribuer au path"), Category = "Path")
	//FName PathName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Point Aleatoire"), Category = "Default")
	FBlackboardKeySelector TargetLoction;

	UPROPERTY(EditAnywhere)
	bool RelativeToSpawn = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = RelativeToSpawn))
	FBlackboardKeySelector SpawnVector;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool DrawDebug = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = DrawDebug), Category = "Debug")
	float DebugRadius = 100.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = DrawDebug), Category = "Debug")
	float DebugDuration = 10.f;

};
