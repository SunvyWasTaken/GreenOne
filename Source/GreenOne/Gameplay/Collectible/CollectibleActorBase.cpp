// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleActorBase.h"

#include "CollectorInterface.h"
#include "Components/BoxComponent.h"

// Sets default values
ACollectibleActorBase::ACollectibleActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = MeshComponent;

	ShapeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Detection Shape"));
	ShapeComponent->SetGenerateOverlapEvents(true);
	ShapeComponent->SetupAttachment(RootComponent);
}

void ACollectibleActorBase::PostCDOCompiled()
{
	Super::PostCDOCompiled();
	// if(TypeShapeComponent)
	// {
	// 	if (UShapeComponent* ShapeExist = FindComponentByClass<UShapeComponent>())
	// 	{
	// 		RemoveInstanceComponent(ShapeExist);
	// 		InitDynamicCollisionShape();
	// 	}else
	// 	{
	// 		InitDynamicCollisionShape();
	// 	}
	// }
}

// Called when the game starts or when spawned
void ACollectibleActorBase::BeginPlay()
{
	Super::BeginPlay();
	if(ShapeComponent)
	{
		ShapeComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACollectibleActorBase::OnCollectibleActorBeginOverlap);
	}
}

void ACollectibleActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(ShapeComponent)
	{
		ShapeComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ACollectibleActorBase::OnCollectibleActorBeginOverlap);
	}
}

void ACollectibleActorBase::OnCollectibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                           bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || !Cast<ICollectorInterface>(OtherActor)) return;
	Action(OtherActor);	
}

// Called every frame
void ACollectibleActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACollectibleActorBase::Action(AActor* Collector)
{
	ICollectibleInterface::Action(Collector);
}

void ACollectibleActorBase::InitDynamicCollisionShape()
{
	ShapeComponent = NewObject<UShapeComponent>(this, TypeShapeComponent);
	ShapeComponent->SetupAttachment(MeshComponent);
	ShapeComponent->SetGenerateOverlapEvents(true);
	ShapeComponent->bAutoRegister = true;
	AddInstanceComponent(ShapeComponent);
	TypeShapeComponent = nullptr;
}
