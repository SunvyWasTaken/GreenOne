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

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	#endif

public:

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* LevelNameText;

	UPROPERTY(EditAnywhere)
	FName LevelName;

	UPROPERTY(EditAnywhere)
	FVector CollisionSize;

	UPROPERTY(EditAnywhere)
	bool ShouldUnload;

private:

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
