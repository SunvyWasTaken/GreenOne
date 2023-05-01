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

	/**
	 * Permet de modifier la forme de la collision,
	 * La forme de collision se reset apres compilation.
	 */
	UPROPERTY(EditDefaultsOnly, DisplayName = "Type de forme de collision")
	TSubclassOf<UShapeComponent> TypeShapeComponent;
	
	UPROPERTY(EditAnywhere)
	UBlueprint* Bp;

#if WITH_EDITOR
	virtual void PostCDOCompiled() override;
#endif
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	virtual void OnCollectibleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnCollectibleActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void InitDynamicCollisionShape();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
