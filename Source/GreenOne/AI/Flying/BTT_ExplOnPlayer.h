#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ExplOnPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBTT_ExplOnPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_ExplOnPlayer();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector SelfRef;

};

