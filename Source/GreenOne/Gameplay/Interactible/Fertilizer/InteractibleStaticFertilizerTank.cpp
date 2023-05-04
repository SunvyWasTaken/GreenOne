// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleStaticFertilizerTank.h"
#include "GreenOne/Gameplay/Weapon/Fertilizer/FertilizerTankComponent.h"

AInteractibleStaticFertilizerTank::AInteractibleStaticFertilizerTank()
{
}

void AInteractibleStaticFertilizerTank::Action(AActor* Interactor)
{
	Super::Action(Interactor);

	if(UFertilizerTankComponent* FertilizerTankComponent = Interactor->FindComponentByClass<UFertilizerTankComponent>())
	{
		if(const FertilizerTankStruct* FertilizerTankStruct = FertilizerTankComponent->GetFertilizerTankByType(InteractibleFertilizerTankStruct.FertilizerType))
		{
			FertilizerTankComponent->SetFertilizerValueByType(InteractibleFertilizerTankStruct.FertilizerType, InteractibleFertilizerTankStruct.GetFertilizerValue(FertilizerTankStruct->GaugeValue));
		}
	}
}
