// Copyright Epic Games, Inc. All Rights Reserved.

#include "GreenOneGameMode.h"
#include "GreenOneCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGreenOneGameMode::AGreenOneGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GreenOne/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
