// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnnemy.h"
#include "GreenOne/Widget/Debug/Debug_W_LifeBar.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ABaseEnnemy::ABaseEnnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LifeBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar_DEBUG"));
	LifeBarComponent->SetupAttachment(RootComponent);
	LifeBarComponent->SetWidgetClass(LifeBarClass);
}

// Called when the game starts or when spawned
void ABaseEnnemy::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = Health;
	if (DrawLifeBar)
	{
		if (LifeBarComponent->GetWidget())
		{
			if (UDebug_W_LifeBar* CurrenLifeBar = Cast<UDebug_W_LifeBar>(LifeBarComponent->GetWidget()))
			{
				CurrenLifeBar->SetOwner(this);
			}
		}
	}
	else
	{
		LifeBarComponent->SetHiddenInGame(true);
	}
}

float ABaseEnnemy::GetPercentHealth()
{
	return Health/MaxHealth;
}

void ABaseEnnemy::EntityTakeDamage_Implementation(float DamageApply)
{
	//UE_LOG(LogTemp, Error, TEXT("Damage On Ennemy"));
	Health -= DamageApply;
	if (Health <= 0.f )	{ Health = 0.f; }
	OnTakeDamage.Broadcast();
}


