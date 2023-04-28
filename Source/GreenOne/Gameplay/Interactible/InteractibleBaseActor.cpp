// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleBaseActor.h"

#include "InteractorInterface.h"
#include "Components/BoxComponent.h"

// Sets default values
AInteractibleBaseActor::AInteractibleBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactible Mesh"));
	if(Mesh)
	{
		RootComponent = Mesh;
	}
	
	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interactible Detection"));
	if(DetectionBox)
	{
		DetectionBox->SetupAttachment(Mesh);
	}
	
	

}

// Called when the game starts or when spawned
void AInteractibleBaseActor::BeginPlay()
{
	Super::BeginPlay();

	if(DetectionBox)
	{
		DetectionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AInteractibleBaseActor::OnInteractibleActorBeginOverlap);
		DetectionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AInteractibleBaseActor::OnIneractibleActorEndOverlap);
	}
}

void AInteractibleBaseActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(DetectionBox)
	{
		DetectionBox->OnComponentBeginOverlap.RemoveDynamic(this, &AInteractibleBaseActor::OnInteractibleActorBeginOverlap);
		DetectionBox->OnComponentEndOverlap.RemoveDynamic(this, &AInteractibleBaseActor::OnIneractibleActorEndOverlap);
	}
}

// Called every frame
void AInteractibleBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractibleBaseActor::OnInteractibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if(!OtherActor) return;
	
	if(IInteractorInterface* Interactor = Cast<IInteractorInterface>(OtherActor))
	{
		Interactor->SetInteractibleActor(this);
	}
}

void AInteractibleBaseActor::OnIneractibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor) return;

	if(IInteractorInterface* Interactor = Cast<IInteractorInterface>(OtherActor))
	{
		Interactor->SetInteractibleActor(nullptr);
	}
}

void AInteractibleBaseActor::Action(AActor* Actor)
{
	if(!Actor || !Cast<IInteractorInterface>(Actor)) return;
	
	IInteractibleActorInterface::Action(Actor);
	UE_LOG(LogTemp, Warning, TEXT("Interaction done"));
}


