// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleStaticActor.h"
#include "Components/BoxComponent.h"

AInteractibleStaticActor::AInteractibleStaticActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = MeshComponent;

	if(DetectionBoxComponent)
	{
		DetectionBoxComponent->SetupAttachment(RootComponent);
	}
}

void AInteractibleStaticActor::Action(AActor* Interactor)
{
	Super::Action(Interactor);
	UE_LOG(LogTemp, Warning, TEXT("Interaction Static done"));
}
