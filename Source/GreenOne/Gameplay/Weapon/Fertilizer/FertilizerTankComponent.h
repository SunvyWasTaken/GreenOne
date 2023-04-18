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

	FertilizerTankStruct();
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100))
	float MaxGaugeValue = 100.f;
	float GaugeValue = MaxGaugeValue;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100))
	float ReduceGaugeValue = 5.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFertilizerBase> Effect;
	
	void UpdateGauge();
	
private:
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
	bool IsTypeExist(const FertilizerType Type) const;
	
public:
	UFUNCTION(BlueprintCallable)
	void OnShoot(const FertilizerType Type);

	UFUNCTION(BlueprintCallable)
	bool IsTankEmpty(const FertilizerType Type);

	UFUNCTION(BlueprintCallable)
	class UFertilizerBase* GetEffect(FertilizerType Type);
	
	FertilizerTankStruct* GetCurrentFertilizerTankActive(FertilizerType Type);
	//TODO: L'effet de l'engrais est propres a son réservoir, si le reservoir est vide alors on ne creer plus d'effet
};
