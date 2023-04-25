// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerFactory.h"

#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerOther.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerSlowDown.h"

FertilizerFactory::FertilizerFactory()
{
}

FertilizerFactory::~FertilizerFactory()
{
}

UFertilizerBase* FertilizerFactory::Factory(FertilizerType Type, TSubclassOf<UFertilizerBase> FertilizerBase)
{
	if(!FertilizerBase)
		return nullptr;
	
	UFertilizerBase* Fertilizer = nullptr;
	switch (Type)
	{
	case FertilizerType::SlowDown:
		Fertilizer = InitFertilizer<UFertilizerSlowDown>(FertilizerBase);
		break;
	case FertilizerType::AttackBonus:
		Fertilizer = InitFertilizer<UFertilizerBase>(FertilizerBase);
		break;
	case FertilizerType::Other:
		Fertilizer = InitFertilizer<UFertilizerOther>(FertilizerBase);
		default:
			break;
	}
	
	return Fertilizer;
}
