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
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UFertilizerTankComponent::BeginPlay()
{
	Super::BeginPlay();

	if(AGreenOneCharacter* Character = Cast<AGreenOneCharacter>(GetOwner()))
	{
		//Character->OnShootDelegate.AddDynamic(this, &UFertilizerTankComponent::OnShoot);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Cast GetOwner, GetOwner is maybe not find !"));
	}
	
}


// Called every frame
void UFertilizerTankComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
#if WITH_EDITOR

	if(bDrawDebugValues)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("Current Fertilizer Type : %s"),*GetFertilizerTypeName()), true, FVector2d(1.2, 1.2));
		Struct = GetCurrentFertilizerTankActive();
		if(Struct)
		{
			GEngine->AddOnScreenDebugMessage(2, .1f, FColor::Green, FString::Printf(TEXT("Fertilizer Max Gauge Value : %f"), Struct->MaxGaugeValue), true, FVector2d(1.2, 1.2));
			GEngine->AddOnScreenDebugMessage(3, .1f, FColor::Red, FString::Printf(TEXT("Fertilizer Reduce Gauge Value : %f"), Struct->ReduceGaugeValue), true, FVector2d(1.2, 1.2));
			GEngine->AddOnScreenDebugMessage(4, .1f, FColor::Blue, FString::Printf(TEXT("Fertilizer Gauge Value : %f"), Struct->GaugeValue), true, FVector2d(1.2, 1.2));
		}	
	}


#endif
	
}

bool UFertilizerTankComponent::IsTypeExist(const FertilizerType Type) const
{
	if(Type == FertilizerType::None) return false;

	if(!FertilizerTanks.Contains(Type)) return false;

	return true;
	
}

void UFertilizerTankComponent::OnShoot()
{
	UE_LOG(LogTemp, Warning, TEXT("OnShoot Update Fertilizer Tank"));
		
	if(FertilizerTankStruct* CurrentFertilizerTankActive = GetCurrentFertilizerTankActive())
	{
		CurrentFertilizerTankActive->UpdateGauge();
		UE_LOG(LogTemp, Warning, TEXT("Current Fertilizer Tank gauge value : %f"),CurrentFertilizerTankActive->GaugeValue);	
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Fertilizer Tank is not exist !"));
	}
}

bool UFertilizerTankComponent::IsTankEmpty(const FertilizerType Type)
{
	if(Type == FertilizerType::None) return true;

	if(const FertilizerTankStruct* CurrentFertilizerTankActive = GetCurrentFertilizerTankActive())
	{
		return CurrentFertilizerTankActive->GaugeValue <= 0 ? true : false;
	}

	return true;
}

void UFertilizerTankComponent::UpdateFertilizerType(FertilizerType Type)
{
	EFertilizerType = Type;
}

FertilizerType UFertilizerTankComponent::GetCurrentFertilizerType() const
{
	return EFertilizerType;
}

UFertilizerBase* UFertilizerTankComponent::GetEffect()
{
	if(!IsTypeExist(EFertilizerType)) return nullptr;

	if(const FertilizerTankStruct* FertilizerTankStruct = GetCurrentFertilizerTankActive())
	{
		if(!FertilizerTankStruct->Effect) return  nullptr;
		
		return FertilizerFactory::Factory(this, EFertilizerType, FertilizerTankStruct->Effect);
	}
	
	return nullptr;
}

FertilizerTankStruct* UFertilizerTankComponent::GetCurrentFertilizerTankActive()
{
	if(!IsTypeExist(EFertilizerType)) return nullptr;
	
	return FertilizerTanks.Find(EFertilizerType);
}

FString UFertilizerTankComponent::GetFertilizerTypeName() const
{
	switch (EFertilizerType)
	{
	case FertilizerType::None:
		return FString(TEXT("Aucun"));
		break;
	case FertilizerType::SlowDown:
		return FString(TEXT("SlowDown"));
		break;
	case FertilizerType::AttackBonus:
		return FString(TEXT("AttackBonus"));
		break;
	}

	return FString(TEXT("Aucun"));
}

