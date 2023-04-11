// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerAttackBonus.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "GreenOne/AI/BaseEnnemy.h"

UFertilizerAttackBonus::UFertilizerAttackBonus() {}
#if WITH_EDITOR
void UFertilizerAttackBonus::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	PercentAtkBonus = FMath::Clamp(PercentAtkBonus, 0, MaxPercentAtkBonus);
}
#endif 
void UFertilizerAttackBonus::ApplyEffect(AActor* Actor, AActor* Source)
{
	if(!Actor) return; if(!Source) return;
	
	AGreenOneCharacter* Chara = Cast<AGreenOneCharacter>(Source);
	ABaseEnnemy* Ennemy = Cast<ABaseEnnemy>(Actor);
	float NbHitOnEnnemy = Ennemy->NumberOfHit;
	
	if(Ennemy && Chara)
	{
		const float Damage = Chara->DamagePlayer;
		
		
		if(Ennemy->GetCharacterMovement())
		{
			if(NbHitOnEnnemy < 5)
			{
				NbHitOnEnnemy += 1;
			}
			float BonusDamage = (Damage * NbHitOnEnnemy * 0.2);
			UE_LOG(LogTemp, Display, TEXT(":Damage %f, BonusDamage: %f"), Damage, BonusDamage);
			Ennemy->EntityTakeDamage(BonusDamage, "none" , Chara);
			Ennemy->NumberOfHit = NbHitOnEnnemy;
		}
	}
}


