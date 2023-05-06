#pragma once

#include "CoreMinimal.h"
#include "FFertilizerTankStruct.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FFertilizerTankStruct
{
	GENERATED_BODY()

	FFertilizerTankStruct();
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties", DisplayName = "Capacite max du reservoir", meta = (ClampMin = 0, ClampMax = 100))
	float MaxGaugeValue = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Custom|Fertilizer|Tank|Properties")
	float GaugeValue = MaxGaugeValue;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties", meta = (ClampMin = 0, ClampMax = 100), DisplayName = "Pourcentage d'engrais")
	float FertilizerValueToGive = 100.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties", DisplayName = "Type d'engrais")
	FertilizerType FertilizerType;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties", DisplayName = "Consome le reservoir")
	bool bIsConsume = false;
	
	float GetFertilizerValue(float CurrentReceverGaugeValue);

private:
	void ClampGauge();
};
