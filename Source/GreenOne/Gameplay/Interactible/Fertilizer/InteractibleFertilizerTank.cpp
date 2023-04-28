// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleFertilizerTank.h"

#include "GreenOne/Gameplay/Weapon/Fertilizer/FertilizerTankComponent.h"

AInteractibleFertilizerTank::AInteractibleFertilizerTank(){
}

void AInteractibleFertilizerTank::Action(AActor* Interactor)
{
	Super::Action(Interactor);

	if(UFertilizerTankComponent* FertilizerTankComponent = Interactor->FindComponentByClass<UFertilizerTankComponent>())
	{
		FertilizerTankStruct* FertilizerTankStruct = FertilizerTankComponent->GetFertilizerTankByType(InteractibleFertilizerTankStruct.FertilizerType);
		if(FertilizerTankStruct)
		{
			FertilizerTankStruct->AddFertilizer(InteractibleFertilizerTankStruct.FertilizerValueToGive);
		}
	}
}
