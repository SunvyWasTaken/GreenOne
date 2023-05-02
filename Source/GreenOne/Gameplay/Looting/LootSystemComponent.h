// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GreenOne/Gameplay/Collectible/CollectibleActorBase.h"
#include "LootSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API ULootSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Custom|Loot")
	TArray<TSubclassOf<ACollectibleActorBase>> Loots;

	UPROPERTY(EditAnywhere, Category = "Custom|Loot", DisplayName = "Taux de drop", meta = (ClampMin = 0, ClampMax = 100, ForceUnits="%"))
	float MaxDropRate;

public:	
	// Sets default values for this component's properties
	ULootSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	void LootItems();
};
