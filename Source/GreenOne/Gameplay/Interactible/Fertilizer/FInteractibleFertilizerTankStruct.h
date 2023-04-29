#pragma once

#include "CoreMinimal.h"
#include "FInteractibleFertilizerTankStruct.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FInteractibleFertilizerTankStruct
{
	GENERATED_BODY()

	FInteractibleFertilizerTankStruct();
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank")
	float MaxGaugeValue = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Custom|Fertilizer|Tank")
	float GaugeValue = MaxGaugeValue;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", meta = (ClampMin = 0, ClampMax = 100), DisplayName = "Pourcentage d'engrais")
	float FertilizerValueToGive = 100.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank", DisplayName = "Type d'engrais")
	FertilizerType FertilizerType;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank")
	bool bIsConsume = false;
	
	float GetFertilizerValue(float CurrentReceverGaugeValue);

private:
	void ClampGauge();
};
