// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerTankComponent.h"

#include "GreenOne/Gameplay/GreenOneCharacter.h"

FertilizerTankStruct::FertilizerTankStruct()
{
	GaugeValue = MaxGaugeValue;
}

void FertilizerTankStruct::UpdateGauge()
{
	GaugeValue -= ReduceGaugeValue;
	ClampGaugeValue();
}

void FertilizerTankStruct::ClampGaugeValue()
{
	if(GaugeValue >= MaxGaugeValue)
	{
		GaugeValue = MaxGaugeValue;
		return;
	}
	
	if(GaugeValue <= 0)
	{
		GaugeValue = 0;
	}	
}

// Sets default values for this component's properties
UFertilizerTankComponent::UFertilizerTankComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFertilizerTankComponent::BeginPlay()
{
	Super::BeginPlay();

	if(AGreenOneCharacter* Character = Cast<AGreenOneCharacter>(GetOwner()))
	{
		Character->OnShootDelegate.AddDynamic(this, &UFertilizerTankComponent::OnShoot);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Cast GetOwner, GetOwner is maybe not find !"));
	}
	
}


// Called every frame
void UFertilizerTankComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UFertilizerTankComponent::OnShoot(FertilizerType Type)
{
	UE_LOG(LogTemp, Warning, TEXT("OnShoot Update Fertilizer Tank"));
		
	if(FertilizerTankStruct* CurrentFertilizerTankActive = GetCurrentFertilizerTankActive(Type))
	{
		CurrentFertilizerTankActive->UpdateGauge();
		UE_LOG(LogTemp, Warning, TEXT("Current Fertilizer Tank gauge value : %f"),CurrentFertilizerTankActive->GaugeValue);	
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fertilizer Tank is not exist !"));
	}
}

FertilizerTankStruct* UFertilizerTankComponent::GetCurrentFertilizerTankActive(FertilizerType Type)
{
	if(Type == FertilizerType::None) return nullptr;

	if(!FertilizerTanks.Contains(Type)) return nullptr;
	
	return FertilizerTanks.Find(Type);
}

