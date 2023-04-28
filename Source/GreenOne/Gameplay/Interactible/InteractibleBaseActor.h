// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleActorInterface.h"
#include "GameFramework/Actor.h"
#include "InteractibleBaseActor.generated.h"

UCLASS()
class GREENONE_API AInteractibleBaseActor : public AActor, public IInteractibleActorInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Custom|Interactible|Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Custom|Interactible|Detection")
	class UBoxComponent* DetectionBox;
	
public:	
	// Sets default values for this actor's properties
	AInteractibleBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnInteractibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnIneractibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Action(IInteractorInterface* Interactor) override;
};