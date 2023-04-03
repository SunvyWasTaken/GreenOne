// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAICharacter.h"

// Sets default values
AMeleeAICharacter::AMeleeAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeleeAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeleeAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMeleeAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AMeleeAICharacter::AttackMeleeOK()
{
	UE_LOG(LogTemp, Warning, TEXT("yo"));
	FHitResult Outhit;
	//IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage, Outhit.BoneName, this);
	
}




