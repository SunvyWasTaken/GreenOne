// Fill out your copyright notice in the Description page of Project Settings.


#include "LootSystemComponent.h"
#include "GreenOne/Gameplay/Collectible/CollectibleActorBase.h"

// Sets default values for this component's properties
ULootSystemComponent::ULootSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULootSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void ULootSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	LootItems();
}

void ULootSystemComponent::LootItems()
{
	for(TSubclassOf<ACollectibleActorBase> Collectible : Loots)
	{
		const float DropRate = FMath::RandRange(0.f, 1.f);
		UE_LOG(LogTemp, Warning, TEXT("Max Drop Rate : %f, Drop Rate : %f"), MaxDropRate, DropRate);
		if(DropRate <= (MaxDropRate/100))
		{
			GetWorld()->SpawnActor<ACollectibleActorBase>(Collectible,GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		}
	}
}

