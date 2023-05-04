#include "BTT_ExplOnPlayer.h"
#include "GreenOne/AI/Flying/FlyingAICharacter.h"
#include "AIController.h"

UBTT_ExplOnPlayer::UBTT_ExplOnPlayer()
{
	bCreateNodeInstance = true;
}

// Optimized and secured version
EBTNodeResult::Type UBTT_ExplOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AFlyingAICharacter* AICharacter = Cast<AFlyingAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		AICharacter->SelfDestruction();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}