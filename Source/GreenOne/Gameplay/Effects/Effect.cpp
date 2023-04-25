// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GreenOne/AI/BaseEnnemy.h"

UEffect::UEffect()
{
}

void UEffect::ApplyEffect(AActor* Actor)
{
	IEffectInterface::ApplyEffect(Actor);
}

void UEffect::ApplyEffect(AActor* Actor, AActor* Source)
{
	if(NSParticleEffect)
	{
		InstantiateParticleToActor(Actor);
	}
}

void UEffect::InstantiateParticleToActor(AActor* Actor)
{
	if(ABaseEnnemy* BaseEnnemy = Cast<ABaseEnnemy>(Actor))
	{
		NiagaraComponentTemp = UNiagaraFunctionLibrary::SpawnSystemAttached(NSParticleEffect,Actor->GetRootComponent(),
			EName::None,Actor->GetActorLocation(),
			Actor->GetActorRotation(),EAttachLocation::KeepWorldPosition,true);

		if(BaseEnnemy->bIsParticleExist(GetParticleEffect()))
		{
			NiagaraComponentTemp->DestroyComponent();
			return;
		}

		BaseEnnemy->AddParticle(GetParticleEffect(), NiagaraComponentTemp);

		if(bTimeEffect)
		{
			BaseEnnemy->ResetEffect(this,GetTimeEffect());
		}
	}
}

const float UEffect::GetTimeEffect()
{
	return TimeEffect;
}

UNiagaraSystem* UEffect::GetParticleEffect() const
{
	if(!NSParticleEffect) return nullptr;
	
	return NSParticleEffect;
}

UNiagaraComponent* UEffect::GetParticleComponent() const
{
	if(!NiagaraComponentTemp) return nullptr;
	
	return NiagaraComponentTemp;
}

void UEffect::DestroyParticleComponent()
{
	if(!NiagaraComponentTemp) return;

	NiagaraComponentTemp->DestroyComponent();
}


