// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackMelee.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UAttackMelee::UAttackMelee()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackMelee::BeginPlay()
{
	Super::BeginPlay();

	CoolDownTimer = MinCoolDown;
	ImpulseForceTemp = ImpulseForce;
	DelayToResetCoolDownTimer = MaxDelayToResetCoolDown;
}


// Called every frame
void UAttackMelee::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetCoolDown(DeltaTime);
	SetDelayToResetCoolDown(DeltaTime);

#if WITH_EDITOR
	
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Blue, FString::Printf(TEXT("CoolDown %f"), CoolDownTimer), true, FVector2d(1.5,1.5));
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Red, FString::Printf(TEXT("CoolDownValue %f"), CoolDown), true, FVector2d(1.5,1.5));

	GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("DelayToResetCoolDown %f"), DelayToResetCoolDownTimer), true, FVector2d(1.5,1.5));
	GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Yellow, FString::Printf(TEXT("DelayToResetCoolDownValue %f"), DelayToResetCoolDown), true, FVector2d(1.5,1.5));

	GEngine->AddOnScreenDebugMessage(5, 1.0f, FColor::Magenta, FString::Printf(TEXT("ImpulseForce %f"), ImpulseForceTemp), true, FVector2d(1.5,1.5));

#endif
}

void UAttackMelee::Attack()
{
	if(!bActiveCoolDown)
		bActiveCoolDown = true;
	else
		return;
	
	DetectActors();
}

void UAttackMelee::DetectActors()
{
	TArray<FHitResult> ActorsHit;
	TArray<AActor*> ActorsIgnores;
	ActorsIgnores.Push(GetOwner());
	
	FVector Start = GetOwner()->GetActorLocation();
	FVector	End = Start + GetOwner()->GetActorForwardVector() * DetectionOffset;
	
	FCollisionShape DetectionConeShape = FCollisionShape::MakeSphere(DetectionRadius);
	
	bool bActorsHit = GetWorld()->SweepMultiByChannel(ActorsHit, End, End, FQuat::Identity, ECC_GameTraceChannel1, DetectionConeShape);
	DrawDebugSphere(GetWorld(), End, DetectionRadius, 8, FColor::Red, false, 2);

	if(bDelayToResetCoolDown)
		ResetCoolDownValues();
	
	UE_LOG(LogTemp, Warning, TEXT("CoolDown %f"), CoolDownTimer);
	
	if(bActorsHit)
		ApplyImpulseForce(ActorsHit);
	
	UE_LOG(LogTemp, Warning, TEXT("Numbers of actors hit : %d"), ActorsHit.Num());
	
	UE_LOG(LogTemp, Warning, TEXT("Attack in component"));
}

void UAttackMelee::ApplyImpulseForce(TArray<FHitResult>& ActorsHit)
{
	for (auto& Actor: ActorsHit)
	{
		if(Actor.GetActor() == GetOwner()) continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Actor to impulse : %s"), *Actor.GetActor()->GetName());
		if(UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(Actor.GetActor()->GetRootComponent()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Impulse %s"), *Actor.GetActor()->GetName());
			Comp->AddImpulse(GetOwner()->GetActorForwardVector() * ImpulseForceTemp, "NAME_None", true);
		}
	}
}

void UAttackMelee::SetCoolDown(float DeltaTime)
{
	if(bActiveCoolDown)
	{
		if(CoolDown < CoolDownTimer)
		{
			CoolDown += DeltaTime;
		}else
		{
			CoolDown = 0;
			bActiveCoolDown = false;
		}
	}
}

void UAttackMelee::SetDelayToResetCoolDown(float DeltaTime)
{
	if(bActiveDelayToResetCoolDown)
	{
		if(DelayToResetCoolDown < DelayToResetCoolDownTimer)
		{
			DelayToResetCoolDown += DeltaTime;
		}else
		{
			DelayToResetCoolDown = 0;
			CoolDownTimer = MinCoolDown;
			ImpulseForceTemp = ImpulseForce;
			bActiveDelayToResetCoolDown = false;
		}
	}
}

void UAttackMelee::ResetCoolDownValues()
{
	if(bActiveDelayToResetCoolDown)
	{
		UE_LOG(LogTemp, Warning, TEXT("DelayCoolDown %f"), DelayToResetCoolDown);
		CoolDownTimer += CoolDownIncrease;
		if(CoolDownTimer > DelayToResetCoolDownTimer-1)
		{
			CoolDownTimer = DelayToResetCoolDownTimer-1;
		}
		DelayToResetCoolDown = 0;
		ImpulseForceTemp -= ImpulseForceReduce;
	}else
	{
		bActiveDelayToResetCoolDown = true;
	}
}
