// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleStaticActor.h"
#include "InteractibleBush.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleBush : public AInteractibleStaticActor
{
	GENERATED_BODY()
	
public:
	AInteractibleBush();

	virtual void Action(AActor* Interactor) override;
};
