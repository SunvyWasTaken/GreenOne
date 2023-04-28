// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SG_GreenOne.generated.h"

/**
 * Save � utiliser dans le jeu.
 */
UCLASS(NotBlueprintable)
class GREENONE_API USG_GreenOne : public USaveGame
{
	GENERATED_BODY()

public:

	/**
	 * Position du player sur la carte.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	FVector PlayerLocation = FVector();

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	FRotator PlayerRotation = FRotator();

	UPROPERTY(BlueprintReadOnly, Category = "Custom|World")
	FName MapName = FName("L_Level0");

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	float PlayerLife = 0;

	/**
	 * Nombre de graine dans le chargeur du player.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	int32 NbrSeed = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|World")
	float SunLocation = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Engine")
	bool bIsFirstTime = true;
	
};
