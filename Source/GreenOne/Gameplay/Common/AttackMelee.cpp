// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "AttackMelee.h"

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
}


// Called every frame
void UAttackMelee::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetCoolDown(DeltaTime);
	SetDelayToResetCoolDown(DeltaTime);
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
			//MeshComp->AddImpulse(FVector::UpVector * ImpulseForce * MeshComp->GetMass(), "NAME_None", true);
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
		if(DelayToResetCoolDown > 0)
		{
			DelayToResetCoolDown -= DeltaTime;
		}else
		{
			DelayToResetCoolDown = MaxDelayToResetCoolDown;
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
		DelayToResetCoolDown = MaxDelayToResetCoolDown;
		CoolDownTimer += CoolDownIncrease;
		ImpulseForceTemp -= ImpulseForceReduce;
	}else
	{
		bActiveDelayToResetCoolDown = true;
	}
}

