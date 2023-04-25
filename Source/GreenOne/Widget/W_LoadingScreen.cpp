// Fill out your copyright notice in the Description page of Project Settings.


#include "W_LoadingScreen.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "GreenOne/Core/Instance/GI_GreenOne.h"
//#include "MoviePlayer.h"


void UW_LoadingScreen::RemoveLoading_Implementation()
{
}

float UW_LoadingScreen::GetLoadingPercent()
{
	return 0.0f;
}

void UW_LoadingScreen::CheckLoadingProcess()
{
	//FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UW_LoadingScreen::BeginLoadingScreen);
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

//This code is setting up a loading screen for a game. The first line creates a FLoadingScreenAttributes object, which is used to store the attributes of the loading
// screen. The next line creates a SNullWidget, which is a widget that does nothing. The following lines create a UUserWidget object, which is a widget that can be
// used to create a custom loading screen. The code then sets the attributes of the loading screen, such as the minimum display time, whether or not the movies are
// skippable, and the widget to be used for the loading screen. Finally, the code calls the GetMoviePlayer() function to set up the loading screen with the attributes
// that were set.
		//FLoadingScreenAttributes LoadingScreen;

		//TSharedRef<SWidget> LoadingScreenWidget = SNullWidget::NullWidget;

		//UUserWidget* BiomeLoadingScreenWidget; // = GetBiomeLoadingScreenWidget(GetActualBiomePlaying());
		//if (BiomeLoadingScreenWidget != nullptr) {
		//	LoadingScreenWidget = BiomeLoadingScreenWidget->TakeWidget();
		//}
		//LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		//LoadingScreen.MinimumLoadingScreenDisplayTime = 1.5f;
		//LoadingScreen.bMoviesAreSkippable = false;

		//LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget;
		//GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

	}


}

void UW_LoadingScreen::BeginLoadingScreen(const FString Name)
{

}

