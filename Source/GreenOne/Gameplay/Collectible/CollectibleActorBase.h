// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollectibleInterface.h"
#include "GameFramework/Actor.h"
#include "CollectibleActorBase.generated.h"

UCLASS()
class GREENONE_API ACollectibleActorBase : public AActor, public ICollectibleInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectibleActorBase();

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UShapeComponent* ShapeComponent;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	virtual void OnCollectibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Action(AActor* Collector) override;
	
};
