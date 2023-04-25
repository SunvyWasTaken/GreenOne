// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GreenOne/AI/BaseEnnemy.h"

UEffect::UEffect()
{
}

void UEffect::Init()
{
	InitMaterialEffect();
}

void UEffect::InitMaterialEffect()
{
	if(MaterialEffect)
	{
		DynamicMaterialEffect = UMaterialInstanceDynamic::Create(MaterialEffect, this);
		if(bOverrideColor)
			DynamicMaterialEffect->SetVectorParameterValue(TEXT("ColorEffect"),ColorMaterialEffect);
	}
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

	if(MaterialEffect && DynamicMaterialEffect)
	{
		ApplyMaterialEffect(Actor);
	}
}

void UEffect::InstantiateParticleToActor(AActor* Actor)
{
	if(ABaseEnnemy* BaseEnnemy = Cast<ABaseEnnemy>(Actor))
	{
		
		if(IsAlreadyExist(Actor))
		{
			if(NiagaraComponentTemp)
				NiagaraComponentTemp->DestroyComponent();
			return;
		}

		NiagaraComponentTemp = UNiagaraFunctionLibrary::SpawnSystemAttached(NSParticleEffect,Actor->GetRootComponent(),
		EName::None,Actor->GetActorLocation(),
		Actor->GetActorRotation(),EAttachLocation::KeepWorldPosition,true);

		if(bTimeEffect)
		{
			BaseEnnemy->ResetEffect(this,GetTimeEffect());
		}
	}
}

void UEffect::ApplyMaterialEffect(const AActor* Actor)
{
	if(!Actor) return;
	
	if(IsMaterialEffectAlreadyExist(Actor)) return;

	if(UMeshComponent* MeshComponent = Actor->FindComponentByClass<UMeshComponent>())
	{
		if(MeshComponent->GetOverlayMaterial()) return;

		MeshComponent->SetOverlayMaterial(DynamicMaterialEffect);
	}

}

float UEffect::GetTimeEffect() const
{
	return TimeEffect;
}

UNiagaraSystem* UEffect::GetParticleEffect() const
{
	if(!NSParticleEffect) return nullptr;
	
	return NSParticleEffect;
}

bool UEffect::IsAlreadyExist(const AActor* Actor)
{
	if(!Actor) return true;

	bool bExist = false;
	
	TArray<UActorComponent*> UActorComponents;
	Actor->GetComponents(UNiagaraComponent::StaticClass(),UActorComponents);
	for (UActorComponent* ActorComponent : UActorComponents)
	{
		if(const UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(ActorComponent))
		{
			if(NiagaraComponent->GetAsset() == GetParticleEffect())
			{
				UE_LOG(LogTemp, Warning, TEXT("Exist"));
				bExist = true;
				break;
			}
		}
	}
	return bExist;
}

bool UEffect::IsMaterialEffectAlreadyExist(const AActor* Actor) const
{
	if(!Actor) return true;

	if(const UMeshComponent* MeshComponent = Actor->FindComponentByClass<UMeshComponent>())
	{
		if(MeshComponent->GetOverlayMaterial())
		{
			return true;
		}		
	}

	return false;
}

UNiagaraComponent* UEffect::GetParticleComponent() const
{
	if(!NiagaraComponentTemp) return nullptr;
	
	return NiagaraComponentTemp;
}
