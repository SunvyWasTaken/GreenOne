#pragma once

#include "CoreMinimal.h"
#include "FFertilizerTankStruct.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FFertilizerTankStruct
{
	GENERATED_BODY()

	FFertilizerTankStruct();
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", DisplayName = "Capacite max du reservoir", meta = (ClampMin = 0, ClampMax = 100, EditCondition="bIsConsume"))
	float MaxGaugeValue = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Custom|Fertilizer|Tank", meta = (EditCondition="bIsConsume"))
	float GaugeValue = MaxGaugeValue;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", meta = (ClampMin = 0, ClampMax = 100), DisplayName = "Pourcentage d'engrais")
	float FertilizerValueToGive = 100.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", DisplayName = "Type d'engrais")
	FertilizerType FertilizerType;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", DisplayName = "Consome le reservoir")
	bool bIsConsume = false;
	
	float GetFertilizerValue(float CurrentReceverGaugeValue);

private:
	void ClampGauge();
};
