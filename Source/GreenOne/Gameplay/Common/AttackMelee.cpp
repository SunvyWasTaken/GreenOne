// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackMelee.h"

// Sets default values for this component's properties
UAttackMelee::UAttackMelee()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAttackMelee::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttackMelee::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttackMelee::Attack(AActor* HitActor, float Damage)
{
	if(!HitActor) return;
	
}

