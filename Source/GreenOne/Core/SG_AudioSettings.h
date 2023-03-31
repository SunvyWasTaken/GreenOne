#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SG_AudioSettings.generated.h"

UCLASS(NotBlueprintable)
class GREENONE_API USG_AudioSettings : public USaveGame
{
	GENERATED_BODY()

public:

	float MasterVolume = 0.7f;

	float MusicVolume = 0.7f;

	float SFXVolume = 0.7f;

};

