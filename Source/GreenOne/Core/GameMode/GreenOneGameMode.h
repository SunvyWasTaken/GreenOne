// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GreenOneGameMode.generated.h"

UCLASS(minimalapi)
class AGreenOneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGreenOneGameMode();

	virtual void BeginPlay();

	UFUNCTION(BlueprintCallable)
	void DisplayGameOver();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom|GameOver")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

};



