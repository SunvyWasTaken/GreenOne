// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"	
#include "NiagaraFunctionLibrary.h"

// Sets default values
AMeleeAICharacter::AMeleeAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//ExploRadius = 100.f;

	ExploDmg = 20.f;
	
}

// Called when the game starts or when spawned
void AMeleeAICharacter::BeginPlay()
{
	Super::BeginPlay();
	//OnTakeDamage.AddDynamic(this, &AMeleeAICharacter::OnDamage);
}

// Called every frame
void AMeleeAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMeleeAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMeleeAICharacter::Collision()
{
	//FHitResult Outhit;
	TArray<FHitResult> ActorsHits;
	TArray<AActor*> ActorsIgnores;
	ActorsIgnores.Push(GetOwner());
	
	FVector OffsetPos = GetActorForwardVector() * Offset.X + GetActorRightVector() * Offset.Y + GetActorUpVector() * Offset.Z;
	FVector Start = GetActorLocation() + OffsetPos;
	
	//FVector Start = GetActorForwardVector() + DetectionOffset;
	//FVector	End = Start + GetOwner()->GetActorForwardVector();
	
	FCollisionShape DetectionConeShape = FCollisionShape::MakeSphere(DetectionRadius);
	GetWorld()->SweepMultiByChannel(ActorsHits, Start, Start, FQuat::Identity, ECC_Camera, DetectionConeShape);
	DrawDebugSphere(GetWorld(), Start, DetectionRadius, 8, FColor::Red, false, 2);


	for (FHitResult ActorsHit : ActorsHits)
	{
		if (!ActorsHit.GetActor()) {}
		if (AGreenOneCharacter* CurrentPlayerRef = Cast<AGreenOneCharacter>(ActorsHit.GetActor()))
		{
			
			CanCombo = true;
			UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *CurrentPlayerRef->GetFName().ToString());
			IEntityGame::Execute_EntityTakeDamage(CurrentPlayerRef, Damage, ActorsHit.BoneName, this);
			/*if (AAIController* AIController = Cast<AAIController>(Controller))
			{
				UBlackboardComponent* BlackboardComp  = AIController->GetBlackboardComponent();
				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsBool("StopMov", false);
					UE_LOG(LogTemp, Warning, TEXT("Stop Mouv"));
				}
			}*/
			break;
		}
	}
}	

void AMeleeAICharacter::StopMov()
{
	if ( CanCombo == true)
	{
		if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool("StopMouv", true);
			}
		}
	}
	else
	{
		if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool("StopMouv", false);
			}
		}
	}

}


/*void AMeleeAICharacter::OnDamage(float value)
{
	if(Health <= 0.f)
	{
		//DrawLifeBar = false;
		DeadEntity();
	}
}*/


