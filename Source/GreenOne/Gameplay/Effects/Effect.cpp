// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UEffect::UEffect()
{
}

void UEffect::ApplyEffect(AActor* Actor)
{
	IEffectInterface::ApplyEffect(Actor);
}

void UEffect::ApplyEffect(AActor* Actor, AActor* Source)
{
	IEffectInterface::ApplyEffect(Actor, Source);
	if(NSParticleEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnParticle"));

		
		UNiagaraFunctionLibrary::SpawnSystemAttached(NSParticleEffect,Actor->GetRootComponent(),
				EName::None,Actor->GetActorLocation(),
				Actor->GetActorRotation(),EAttachLocation::KeepWorldPosition,true);
	
	}
}

const float UEffect::GetTimeEffect()
{
	return TimeEffect;
}
