// Copyright Epic Games, Inc. All Rights Reserved.

#include "GreenOneGameMode.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GreenOne/Core/GI_GreenOne.h"

AGreenOneGameMode::AGreenOneGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GreenOne/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGreenOneGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (AGreenOneCharacter* CharaRef = Cast<AGreenOneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		CharaRef->OnPlayerDeath.AddDynamic(this, &AGreenOneGameMode::DisplayGameOver);
	}
	Cast<UGI_GreenOne>(UGameplayStatics::GetGameInstance(GetWorld()))->ApplySaveData();
}

void AGreenOneGameMode::LoadOneLevel(TSoftObjectPtr<UWorld> LevelToUnload, TSoftObjectPtr<UWorld> LevelToLoad)
{
	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), LevelToUnload, LatentInfo, false);
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), LevelToLoad, true, true, LatentInfo);
}

void AGreenOneGameMode::DisplayGameOver()
{
	if (GameOverWidgetClass)
	{
		UUserWidget* WidgetRef = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		WidgetRef->AddToViewport();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetRef);
	}
}
