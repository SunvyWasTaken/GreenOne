// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_GetRandPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTT_GetRandPoint::UBTT_GetRandPoint()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_GetRandPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* SelfRef = Cast<AController>(OwnerComp.GetOwner());
	if (!SelfRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelfRef Failed"));
		return EBTNodeResult::Failed;
	}
	FVector TargetPoint;
	switch (SearchType)
	{
	case EZoneType::Area:
		TargetPoint = UKismetMathLibrary::RandomPointInBoundingBox((RelativeToSpawn) ? (OwnerComp.GetBlackboardComponent()->GetValueAsVector(SpawnVector.SelectedKeyName)) : (SelfRef->GetPawn()->GetActorLocation()), FVector(SearchRadArea));
		break;
	case EZoneType::Path:
		//UBlackboardComponent* BlackBoardRef = UAIBlueprintHelperLibrary::GetBlackboard(OwnerComp.GetOwner());
		//if (ADronePath* PathRef = Cast<ADronePath>(BlackBoardRef->GetValueAsObject(PathName)))
		//{
		//	TargetPoint = UKismetMathLibrary::RandomPointInBoundingBox(PathRef->GetPoint(), FVector(SearchRadArea));
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Aucun Path actor n'as était assigner"));
		//	return EBTNodeResult::Aborted;
		//}
		break;
	}

	// What should I do if the target is in a wall or on the Other side?
	// For the moment it will fail. Since it's just the patrol in the BT.
	//FHitResult Outhit;
	//if(GetWorld()->LineTraceSingleByChannel(Outhit, SelfRef->GetPawn()->GetActorLocation(), TargetPoint, ECC_Visibility))
	//{
	//	UE_LOG(LogTemp,Warning, TEXT("Faild in the get random Points there is a collision in the path."));
	//	return EBTNodeResult::Failed;
	//}
	if (DrawDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetPoint, DebugRadius, 12, FLinearColor::Red, DebugDuration);
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLoction.SelectedKeyName, TargetPoint);
	return EBTNodeResult::Succeeded;
}
