// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleBaseActorComponent.h"

#include "InteractorInterface.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UInteractibleBaseActorComponent::UInteractibleBaseActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractibleBaseActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if(UShapeComponent* ShapeComponent = GetOwner()->FindComponentByClass<UShapeComponent>())
	{
		ShapeComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractibleBaseActorComponent::OnInteractibleActorBeginOverlap);
		ShapeComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractibleBaseActorComponent::OnIneractibleActorEndOverlap);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Actor : Detection Component not found!!"), *GetOwner()->GetName());
	}
	
}

void UInteractibleBaseActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if(UShapeComponent* ShapeComponent = GetOwner()->FindComponentByClass<UShapeComponent>())
	{
		ShapeComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractibleBaseActorComponent::OnInteractibleActorBeginOverlap);
		ShapeComponent->OnComponentEndOverlap.RemoveDynamic(this, &UInteractibleBaseActorComponent::OnIneractibleActorEndOverlap);
	}
}

// Called every frame
void UInteractibleBaseActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractibleBaseActorComponent::OnInteractibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                                      AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                      const FHitResult& SweepResult)
{
	if(!OtherActor) return;
	
	if(IInteractorInterface* Interactor = Cast<IInteractorInterface>(OtherActor))
	{
		Interactor->SetInteractibleActor(this);
	}
}

void UInteractibleBaseActorComponent::OnIneractibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor) return;

	if(IInteractorInterface* Interactor = Cast<IInteractorInterface>(OtherActor))
	{
		Interactor->SetInteractibleActor(nullptr);
	}
}

void UInteractibleBaseActorComponent::Action(AActor* Actor)
{
	IInteractibleActorInterface::Action(Actor);

	if(!Actor || !Cast<IInteractorInterface>(Actor)) return;
	UE_LOG(LogTemp, Warning, TEXT("Interaction Component done"));
}
