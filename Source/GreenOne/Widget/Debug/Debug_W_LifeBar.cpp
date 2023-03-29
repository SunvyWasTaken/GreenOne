// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug_W_LifeBar.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "Components/ProgressBar.h"


void UDebug_W_LifeBar::SetOwner(AActor* OwnerRef)
{
	CurrentOwner = Cast<ABaseEnnemy>(OwnerRef);
	if (!CurrentOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("Attention l'owner de Debug_W_LifeBar n'est pas enfant de la class BaseEnnemy!!!"));
		return;
	}
	CurrentOwner->OnTakeDamage.AddDynamic(this, &UDebug_W_LifeBar::UpdateBar);
	UpdateBar();
}

void UDebug_W_LifeBar::UpdateBar()
{
	//UE_LOG(LogTemp, Warning, TEXT("Update Life Bar"));
	LifeBar->SetPercent(CurrentOwner->GetPercentHealth());
}