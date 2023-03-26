// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackMelee.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UAttackMelee : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackMelee();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Fight")
	void Attack(AActor* HitActor, float Damage);

private:
	UPROPERTY(EditAnywhere, Category = "Fight|Melee")
	float ImpulseForce = 0.f;

	UPROPERTY(EditAnywhere, Category = "Fight|Detection")
	float RangeDetection = 150.f;

	UPROPERTY(EditAnywhere, Category = "Fight|Detection")
	float RadiusDetection = 25.f;
	
};
