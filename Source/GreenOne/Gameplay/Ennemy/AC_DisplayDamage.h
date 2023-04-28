// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_DisplayDamage.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UAC_DisplayDamage : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_DisplayDamage();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, Category = "Custom")
	TSubclassOf<class ATextRenderActor> TextDamage;

private:

	UFUNCTION()
	void DisplayDamage(float NbrDamage);

		
};
