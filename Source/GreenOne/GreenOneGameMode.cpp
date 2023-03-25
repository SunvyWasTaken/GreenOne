// Copyright Epic Games, Inc. All Rights Reserved.

#include "GreenOneGameMode.h"
#include "GreenOneCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AGreenOneGameMode::AGreenOneGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GreenOne/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGreenOneGameMode::LoadOneLevel(TSoftObjectPtr<UWorld> LevelToUnload, TSoftObjectPtr<UWorld> LevelToLoad)
{
	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), LevelToUnload, LatentInfo, false);
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), LevelToLoad, true, true, LatentInfo);
}
