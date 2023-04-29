// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleBaseActor.h"
#include "InteractibleStaticActor.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleStaticActor : public AInteractibleBaseActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	
public:
	AInteractibleStaticActor();

	virtual void Action(AActor* Actor) override;
};
