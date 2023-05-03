// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleSkeletonActor.h"
#include "Components/BoxComponent.h"

AInteractibleSkeletonActor::AInteractibleSkeletonActor()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Static Mesh"));
	RootComponent = MeshComponent;

	if(DetectionBoxComponent)
	{
		DetectionBoxComponent->SetupAttachment(RootComponent);
	}
}

void AInteractibleSkeletonActor::Action(AActor* Actor)
{
	Super::Action(Actor);
	UE_LOG(LogTemp, Warning, TEXT("Interaction Skeleton done"));
}
