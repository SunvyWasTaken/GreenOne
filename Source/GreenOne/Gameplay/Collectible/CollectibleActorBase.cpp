// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleActorBase.h"
#include "Components/BoxComponent.h"

// Sets default values
ACollectibleActorBase::ACollectibleActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = MeshComponent;
}

void ACollectibleActorBase::PostCDOCompiled()
{
	Super::PostCDOCompiled();
	if(TypeShapeComponent)
	{
		if (UShapeComponent* ShapeExist = FindComponentByClass<UShapeComponent>())
		{
			RemoveInstanceComponent(ShapeExist);
			InitDynamicCollisionShape();
		}else
		{
			InitDynamicCollisionShape();
		}
	}
}

// Called when the game starts or when spawned
void ACollectibleActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACollectibleActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACollectibleActorBase::OnCollectibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                           bool bFromSweep, const FHitResult& SweepResult)
{
}

void ACollectibleActorBase::OnCollectibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void ACollectibleActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACollectibleActorBase::InitDynamicCollisionShape()
{
	ShapeComponent = NewObject<UShapeComponent>(this, TypeShapeComponent);
	ShapeComponent->SetupAttachment(RootComponent);
	ShapeComponent->bAutoRegister = true;
	AddInstanceComponent(ShapeComponent);
	TypeShapeComponent = nullptr;
}
