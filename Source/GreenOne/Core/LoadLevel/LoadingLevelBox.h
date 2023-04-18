// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadingLevelBox.generated.h"

UCLASS()
class GREENONE_API ALoadingLevelBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALoadingLevelBox();

protected:

	virtual void BeginPlay();


public:

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UTextRenderComponent* LevelNameText;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Custom|Level")
	TSoftObjectPtr<UWorld> LevelToLoad;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Custom|Level")
	TSoftObjectPtr<class APlayerStart> PlayerStartRef;


private:

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
