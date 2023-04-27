// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Debug_W_LifeBar.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UDebug_W_LifeBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* LifeBar;

	UFUNCTION(BlueprintCallable)
	void SetOwner(class AActor* OwnerRef);

private:

	class ABaseEnnemy* CurrentOwner;

	UFUNCTION()
	void UpdateBar(float NbrDamage);
	
};
