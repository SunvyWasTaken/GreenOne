// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleBaseActor.h"
#include "InteractibleSkeletonActor.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleSkeletonActor : public AInteractibleBaseActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* MeshComponent;

public:
	AInteractibleSkeletonActor();

	virtual void Action(AActor* Actor) override;
};
