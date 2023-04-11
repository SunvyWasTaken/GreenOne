// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect.h"

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
}
