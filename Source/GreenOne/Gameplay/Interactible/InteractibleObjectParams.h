// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractibleObjectParams.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UInteractibleObjectParams : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractibleObjectParams();


protected:
	
	UPROPERTY(EditAnywhere, Category = "Custom|Mesh|Params")
	TSubclassOf<UMeshComponent> ParamMeshComponent;
	
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
