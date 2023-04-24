// Fill out your copyright notice in the Description page of Project Settings.


#include "W_LoadingScreen.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GreenOne/Core/Instance/GI_GreenOne.h"


void UW_LoadingScreen::RemoveLoading_Implementation()
{
}

float UW_LoadingScreen::GetLoadingPercent()
{
	return 0.0f;
}

void UW_LoadingScreen::CheckLoadingProcess()
{
	UGI_GreenOne* truc = Cast<UGI_GreenOne>(GetGameInstance());
	if (TInfo != nullptr)
	{
		//const ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("L_Level1"));
		//StreamingLevel->GetLoadedLevelPackageName()
		TSet<int32> UIDS;
		GetWorld()->GetLatentActionManager().GetActiveUUIDs(GetWorld(), UIDS);
		UE_LOG(LogTemp, Warning, TEXT("Je charge."));
		for (int32 UID : UIDS)
		{
			UE_LOG(LogTemp, Error, TEXT("UUID active : %d"), UID);
		}
		int32 NbrLoadingObject = GetWorld()->GetLatentActionManager().GetNumActionsForObject(GetGameInstance());
		TInfo->SetText(FText::FromString(FString::FromInt(NbrLoadingObject)));
		//TInfo->SetText(FText::FromString(GetWorld()->GetLatentActionManager().GetDescription(GetWorld(), truc->LatentInfo.UUID)));

		//if (StreamingLevel != nullptr && StreamingLevel->IsStreamingStatePending())
		//{
		//	FString LevelName = StreamingLevel->GetWorldAssetPackageName();
		//	UE_LOG(LogTemp, Warning, TEXT("Currently loading level: %s"), *LevelName);
		//}

	}


}

