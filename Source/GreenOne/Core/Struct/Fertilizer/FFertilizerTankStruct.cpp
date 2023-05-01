#include "FFertilizerTankStruct.h"

FFertilizerTankStruct::FFertilizerTankStruct()
{
	
}

float FFertilizerTankStruct::GetFertilizerValue(float CurrentReceverGaugeValue)
{
	float ValueToReturn = FertilizerValueToGive;
	if(bIsConsume)
	{
		if(GaugeValue <= 0) return 0;
		if(GaugeValue >= FertilizerValueToGive)
		{
			const float TempNeedValue = (MaxGaugeValue - CurrentReceverGaugeValue);
			if(FertilizerValueToGive <= TempNeedValue)
			{
				GaugeValue -= FertilizerValueToGive;
				ValueToReturn = FertilizerValueToGive;
			}else
			{
				GaugeValue -= TempNeedValue;
				ValueToReturn = TempNeedValue;
			}
		}else
		{
			ValueToReturn = GaugeValue;
			GaugeValue-= GaugeValue;
		}
		
	}

	ClampGauge();
	return ValueToReturn;
}

void FFertilizerTankStruct::ClampGauge()
{
	if(bIsConsume)
	{
		if(GaugeValue <= 0.f)
		{
			GaugeValue = 0.f;
		}
	}
}
