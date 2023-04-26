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
	PercentSlowDown = FMath::Clamp(PercentSlowDown, 0, MaxPercentSlownDown);
}
#endif

void UFertilizerSlowDown::ApplyEffect(AActor* Actor, AActor* Source)
{
	Super::ApplyEffect(Actor, Source);
	if(!Actor) return;

	if(ABaseEnnemy* Ennemy = Cast<ABaseEnnemy>(Actor))
	{
		if(Ennemy->GetCharacterMovement())
		{
			const float ActorMaxSpeed = Ennemy->MaxSpeed;
			const float ActorCurrentSpeed = Ennemy->GetCharacterMovement()->GetMaxSpeed();
			const float ReduceSpeed = (ActorMaxSpeed*PercentSlowDown)/100;
			const float NewSpeed = ActorCurrentSpeed-ReduceSpeed;
			UE_LOG(LogTemp, Warning, TEXT("new speed %f"), NewSpeed);
			if(GetTotalPercent(ActorMaxSpeed, ActorCurrentSpeed) < MaxPercentSlownDown)
			{
				Ennemy->UpdateMaxSpeed(NewSpeed);	
			}
			UE_LOG(LogTemp, Warning, TEXT("SlowDown, MaxSpeedActor = %f, ReduceSpeed = %f, CalculPercentSlowDown = %f"), ActorCurrentSpeed, ReduceSpeed, GetTotalPercent(ActorMaxSpeed,ActorCurrentSpeed));

			if(bTimeEffect)
			{
				Ennemy->ResetEffect(this,GetTimeEffect());
			}
		}		
	}

}

float UFertilizerSlowDown::GetTotalPercent(float Value, float ReduceValue)
{
	return ((Value-ReduceValue)*100)/Value;
}
