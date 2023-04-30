// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleActorInterface.h"
#include "GameFramework/Actor.h"
#include "InteractibleBaseActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractActorSignature, bool, Display);

UCLASS()
class GREENONE_API AInteractibleBaseActor : public AActor, public IInteractibleActorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractibleBaseActor();

	UPROPERTY(BlueprintAssignable)
	FOnInteractActorSignature OnInteractActorDelegate;

protected:

	UPROPERTY(EditAnywhere)
	class UBoxComponent* DetectionBoxComponent;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	virtual void OnInteractibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnIneractibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetupInteractibleToInteractor(bool Setup, AActor* Interactor);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Action(AActor* Interactor) override;

};