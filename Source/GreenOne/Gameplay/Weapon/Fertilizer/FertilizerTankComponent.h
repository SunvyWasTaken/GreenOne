// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FertilizerTankComponent.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FertilizerTankStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FertilizerType TankFertilizerType;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100))
	float MaxGaugeValue = 100;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100))
	float ReduceGaugeValue = 5;

private:
	float GaugeValue;
	void ClampGaugeValue();
	
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UFertilizerTankComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFertilizerTankComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
	TMap<FertilizerType,FertilizerTankStruct> FertilizerTanks;

public:
	UFUNCTION(BlueprintCallable)
	void OnShoot(FertilizerType Type);

};
