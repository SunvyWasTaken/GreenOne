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


/*void AMeleeAICharacter::AttackMeleeOK()
{
	//Create an array to store the results of the trace
	TArray<FHitResult> Outhits;

	//Create an array to store the actors to ignore
	TArray<AActor*> ActorToIgnore;

	//Add the current actor to the array of actors to ignore
	ActorToIgnore.Add(this);


	//Perform a sphere trace to detect any actors within the explosion radius
	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), ExploRadius, UCollisionProfile::Get()->ConvertToTraceType(ECC_Camera), false, ActorToIgnore, EDrawDebugTrace::None, Outhits, true))
	{
		//Loop through the results of the trace
		for (FHitResult Outhit : Outhits)
		{
			//Check if nothing was hit by the explosion
			if (!Outhit.GetActor())
			{
				UE_LOG(LogTemp, Warning, TEXT("nothing hit by the explosion."));
			}
			//Check if the actor hit is a GreenOneCharacter
			if (AGreenOneCharacter* CurrentPlayerRef = Cast<AGreenOneCharacter>(Outhit.GetActor()))
			{
				UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *CurrentPlayerRef->GetFName().ToString());
				//Call the EntityTakeDamage function on the GreenOneCharacter
				IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage, Outhit.BoneName, this);
				//Break out of the loop
				break;
			}
		}
	}
	//Check if the ExplosionParticule is not null
	if (ExplosionParticule != nullptr)
	{
		//Spawn the ExplosionParticule at the actor's location
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionParticule, GetActorLocation());
	}

	
	/*UE_LOG(LogTemp, Warning, TEXT("yo"));
	FHitResult Outhit;
	//IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage, Outhit.BoneName, this);

}*/


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
			if (AAIController* AIController = Cast<AAIController>(Controller))
			{
				UBlackboardComponent* BlackboardComp  = AIController->GetBlackboardComponent();
				if (BlackboardComp)
				{
					BlackboardComp->SetValueAsBool("StopMov", false);
					UE_LOG(LogTemp, Warning, TEXT("Stop Mouv"));
				}
			}
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


