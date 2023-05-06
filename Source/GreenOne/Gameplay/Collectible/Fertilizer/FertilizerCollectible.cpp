// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerCollectible.h"
#include "GreenOne/Gameplay/Weapon/Fertilizer/FertilizerTankComponent.h"

AFertilizerCollectible::AFertilizerCollectible()
{
	
}

#if WITH_EDITOR
void AFertilizerCollectible::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AFertilizerCollectible::Action(AActor* Collector)
{
	Super::Action(Collector);

	if(UFertilizerTankComponent* FertilizerTankComponent = Collector->FindComponentByClass<UFertilizerTankComponent>())
	{
		if(FertilizerTankStruct* FertilizerTankStruct = FertilizerTankComponent->GetFertilizerTankByType(CollectibleFertilizerTankStruct.FertilizerType)
)
		{
			FertilizerTankStruct->AddFertilizer(CollectibleFertilizerTankStruct.GetFertilizerValue(FertilizerTankStruct->GaugeValue));
		}
	}
	Destroy();
}
