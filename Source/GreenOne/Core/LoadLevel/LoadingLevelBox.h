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

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* LevelNameText;

	UPROPERTY(EditAnywhere)
	bool ShouldUnload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly	)
	TSoftObjectPtr<UWorld> LevelToUnload;

private:

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
