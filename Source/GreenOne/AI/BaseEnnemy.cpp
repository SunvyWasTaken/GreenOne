// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnnemy.h"
#include "GreenOne/Widget/Debug/Debug_W_LifeBar.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "EnnemySpawner.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	return Health / MaxHealth;
}

void ABaseEnnemy::EnityTakeEffect_Implementation(UEffect* Effect, AActor* Source)
{
	if(!Effect) return;
	Effect->ApplyEffect(this);
}

void ABaseEnnemy::SetPlayerRef(AActor* ref)
{
	if (AAIController* AIController = Cast<AAIController>(Controller))
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject("TargetPlayer", ref);
		}
	}
}

void ABaseEnnemy::EntityTakeDamage_Implementation(float DamageApply, FName BoneNameHit, AActor* DamageSource = nullptr)
{
	//UE_LOG(LogTemp, Error, TEXT("Damage On Ennemy"));
	float CurrentDamageMulti = 1.f;
	if (!ListWeakPoint.IsEmpty())
	{
		for (FName& var : ListWeakPoint)
		{
			if (BoneNameHit == var)
			{
				CurrentDamageMulti = CritMultiplyer;
				break;
			}
		}
	}
	Health -= DamageApply * CurrentDamageMulti;
	if (IsFriendlyFire)
	{
		Cast<AAIController>(Controller)->GetBlackboardComponent()->SetValueAsObject("TargetPlayer", DamageSource);
	}
	if (LifeTreshold.Num() == MatTreshold.Num())
	{
		ChangeTextureBaseHealth();
	}
	if (Health <= 0.f)
	{
		Health = 0.f;
		DeadEntity();
	}
	OnTakeDamage.Broadcast();
}

void ABaseEnnemy::ChangeTextureBaseHealth()
{
	if (!LifeTreshold.IsEmpty())
	{
		for (int32 i = 0; i < LifeTreshold.Num(); ++i)
		{
			if (Health <= LifeTreshold[i])
			{
				this->GetMesh()->SetMaterial(0, MatTreshold[i]);
			}
		}
	}
}

void ABaseEnnemy::DeadEntity()
{
	GetMesh()->SetSimulatePhysics(true);
	Cast<AAIController>(GetController())->GetBrainComponent()->StopLogic("Because");
	if (SpawnerRef != nullptr)
	{
		SpawnerRef->RemoveEntityFromList(this);
	}
}

