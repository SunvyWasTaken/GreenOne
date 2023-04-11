// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackMelee.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	if(AGreenOneCharacter* Character = Cast<AGreenOneCharacter>(GetOwner()))
	{
		Character->IsAtk = true;
		
	}
	
	if(!bActiveCoolDown)
		bActiveCoolDown = true;
	else
		return;
	
	DetectActors();
}

void UAttackMelee::Conetrace(TArray<FHitResult>& TargetHits)
{
	const unsigned Iteration = 5;
	const float TraceSize = (TraceDistance / Iteration);
	const FVector StartPosition = GetOwner()->GetActorLocation();
	const FVector ForwardActor = GetOwner()->GetActorForwardVector();
	const FRotator ActorRotation = GetOwner()->GetActorRotation();
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(GetOwner());
	for (int i = 1; i <= Iteration; ++i)
	{
		float CurrentAlpha = UKismetMathLibrary::NormalizeToRange((TraceSize * i), 0, TraceDistance);
		float BoxSize = UKismetMathLibrary::Lerp(0, ConeRadius, CurrentAlpha);
		FVector CurrentPos = StartPosition + (ForwardActor * (TraceSize*2) * i+1);
		FVector Box = FVector(TraceSize, BoxSize, ConeHeight);
		TArray<FHitResult> CurrentHits;
		UKismetSystemLibrary::BoxTraceMulti(GetWorld(), CurrentPos, CurrentPos, Box, ActorRotation, UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::Persistent, CurrentHits, true);
		TargetHits += CurrentHits;
	}
}

void UAttackMelee::DetectActors()
{
	TArray<FHitResult> ActorsHit;
	TArray<AActor*> ActorsIgnores;
	ActorsIgnores.Push(GetOwner());
	
	FVector Start = GetOwner()->GetActorLocation();
	FVector	End = Start + GetOwner()->GetActorForwardVector() * DetectionOffset;
	
	FCollisionShape DetectionConeShape = FCollisionShape::MakeSphere(DetectionRadius);
	Conetrace(ActorsHit);
	bool bActorsHit = false;
	// bool bActorsHit = GetWorld()->SweepMultiByChannel(ActorsHit, End, End, FQuat::Identity, ECC_GameTraceChannel1, DetectionConeShape);
	DrawDebugSphere(GetWorld(), End, DetectionRadius, 8, FColor::Red, false, 2);

	if(bDelayToResetCoolDown)
		ResetCoolDownValues();
	
	UE_LOG(LogTemp, Warning, TEXT("CoolDown %f"), CoolDownTimer);
	
	if(bActorsHit)
		ApplyImpulseForce(ActorsHit);
	
	UE_LOG(LogTemp, Warning, TEXT("Numbers of actors hit : %d"), ActorsHit.Num());
	
	UE_LOG(LogTemp, Warning, TEXT("Attack in component"));
	if(AGreenOneCharacter* Character = Cast<AGreenOneCharacter>(GetOwner()))
	{
		Character->IsAtk = false;
	}
}

void UAttackMelee::ApplyImpulseForce(TArray<FHitResult>& ActorsHit)
{
	for (auto& Actor: ActorsHit)
	{
		if(Actor.GetActor() == GetOwner()) continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Actor to impulse : %s"), *Actor.GetActor()->GetName());
		if(UCharacterMovementComponent* Comp = Cast<UCharacterMovementComponent>(Cast<ACharacter>(Actor.GetActor())->GetCharacterMovement()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Impulse %s"), *Actor.GetActor()->GetName());
			Comp->AddImpulse(GetOwner()->GetActorForwardVector() * ImpulseForceTemp, true);
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
		ReduceImpusleForce();
	}else
	{
		bActiveDelayToResetCoolDown = true;
	}
}

void UAttackMelee::ReduceImpusleForce()
{
	ImpulseForceTemp<=0?0:ImpulseForceTemp -= ImpulseForceReduce;
}
