// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerSlowDown.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GreenOne/AI/BaseEnnemy.h"

UFertilizerSlowDown::UFertilizerSlowDown()
{
}

#if WITH_EDITOR
void UFertilizerSlowDown::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	PercentSlowDown = FMath::Clamp(PercentSlowDown, 0, MaxPercentSlownDown);
}
#endif

void UFertilizerSlowDown::ApplyEffect(AActor* Actor)
{
	if(!Actor) return;

	if(ABaseEnnemy* Ennemy = Cast<ABaseEnnemy>(Actor))
	{
		if(Ennemy->GetCharacterMovement())
		{
			const float ActorMaxSpeed = Ennemy->MaxSpeed;
			const float ActorCurrentSpeed = Ennemy->GetCharacterMovement()->GetMaxSpeed();
			const float ReduceSpeed = (ActorCurrentSpeed*PercentSlowDown)/100;
			const float NewSpeed = ActorCurrentSpeed-ReduceSpeed;
			if(GetTotalPercent(ActorMaxSpeed, ActorCurrentSpeed) < MaxPercentSlownDown)
			{
				Ennemy->UpdateMaxSpeed(NewSpeed);	
			}
			UE_LOG(LogTemp, Warning, TEXT("SlowDown, MaxSpeedActor = %f, ReduceSpeed = %f, CalculPercentSlowDown = %f"), ActorCurrentSpeed, ReduceSpeed, GetTotalPercent(ActorMaxSpeed,ActorCurrentSpeed));
		}		
	}

}

float UFertilizerSlowDown::GetTotalPercent(float Value, float ReduceValue)
{
	return ((Value-ReduceValue)*100)/Value;
}
