// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleSkFertilizerTank.h"
#include "GreenOne/Gameplay/Weapon/Fertilizer/FertilizerTankComponent.h"

AInteractibleSkFertilizerTank::AInteractibleSkFertilizerTank()
{
}

void AInteractibleSkFertilizerTank::Action(AActor* Interactor)
{
	Super::Action(Interactor);
    
    	if(UFertilizerTankComponent* FertilizerTankComponent = Interactor->FindComponentByClass<UFertilizerTankComponent>())
    	{
    		if(FertilizerTankStruct* FertilizerTankStruct = FertilizerTankComponent->GetFertilizerTankByType(InteractibleFertilizerTankStruct.FertilizerType)
    )
    		{
    			FertilizerTankStruct->AddFertilizer(InteractibleFertilizerTankStruct.GetFertilizerValue(FertilizerTankStruct->GaugeValue));
    		}
    	}
}
