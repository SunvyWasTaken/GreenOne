#pragma once

#include "CoreMinimal.h"
#include "Effects/Effect.h"
#include "UObject/Interface.h"
#include "EntityGame.generated.h"

UINTERFACE(MinimalAPI)
class UEntityGame : public UInterface
{
	GENERATED_BODY()
};

class GREENONE_API IEntityGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EntityTakeDamage(float damage, FName BoneNameHit, AActor* DamageSource = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnityTakeEffect(UEffect* Effect, AActor* Source = nullptr);
};