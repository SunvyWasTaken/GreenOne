// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerEffectFactory.h"

#include "FertilizerOther.h"

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
		Fertilizer = NewObject<UFertilizerSlowDown>(FertilizerBase);
		break;
	case FertilizerType::Other:
		Fertilizer = NewObject<UFertilizerOther>(FertilizerBase);
		default:
			break;
	}
	
	return Fertilizer;
}
