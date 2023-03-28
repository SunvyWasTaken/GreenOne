#include "BTT_ExplOnPlayer.h"
#include "GreenOne/AI/Flying/FlyingAICharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_ExplOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AFlyingAICharacter* SelfPawnRef = Cast<AFlyingAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		SelfPawnRef->SelfDestruction();
	}
	return EBTNodeResult::Succeeded;
}