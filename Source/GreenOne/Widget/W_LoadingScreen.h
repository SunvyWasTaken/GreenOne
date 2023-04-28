// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_LoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UW_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TInfo;

	UFUNCTION(BlueprintNativeEvent)
	void RemoveLoading();

	float GetLoadingPercent();

	UFUNCTION(BlueprintCallable)
		void CheckLoadingProcess();

private:

	UFUNCTION()
		void BeginLoadingScreen(const FString Name);

};
