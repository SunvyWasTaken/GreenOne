// Fill out your copyright notice in the Description page of Project Settings.


#include "W_LoadingScreen.h"
#include "Components/TextBlock.h"


float UW_LoadingScreen::GetLoadingPercent()
{
	return 0.0f;
}

void UW_LoadingScreen::CheckLoadingProcess()
{
	if (TInfo != nullptr)
	{
		TInfo->SetText(FText::FromString(GetWorld()->GetLatentActionManager().GetDescription(GetWorld(), 0)));
	}
}

