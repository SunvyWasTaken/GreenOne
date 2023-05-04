// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"	
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMeleeAICharacter::AMeleeAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//ExploRadius = 100.f;
	R_ArmCollider = FindComponentByClass<UBoxComponent>();
	L_ArmCollider = FindComponentByClass<UBoxComponent>();
	//L_ArmCollider->OnComponentHit.AddDynamic(this, &AMeleeAICharacter::OnCompHit);
	ExploDmg = 20.f;
	
	R_ArmCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("R_ArmCollider"));
	R_ArmCollider->SetupAttachment(GetMesh(),FName("lowerarm_rSocket"));
	L_ArmCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("L_ArmCollider"));
	L_ArmCollider->SetupAttachment(GetMesh(), FName("upperarm_lSocket"));
	
}

// Called when the game starts or when spawned
void AMeleeAICharacter::BeginPlay()
{
	Super::BeginPlay();
	//OnTakeDamage.AddDynamic(this, &AMeleeAICharacter::OnDamage);
	
	R_ArmCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMeleeAICharacter::OnCompHit);
	L_ArmCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMeleeAICharacter::OnCompHit);
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

void AMeleeAICharacter::SetCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Enable collision"));
	if(L_ArmCollider)
	{
		L_ArmCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		L_ArmCollider->SetGenerateOverlapEvents(true);
	}
	if(R_ArmCollider)
	{
		R_ArmCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		R_ArmCollider->SetGenerateOverlapEvents(true);
	}
}

void AMeleeAICharacter::EndCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("Disable collision"));
	if(L_ArmCollider)
	{
		L_ArmCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		L_ArmCollider->SetGenerateOverlapEvents(false);
	}
	if(R_ArmCollider)
	{
		R_ArmCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		R_ArmCollider->SetGenerateOverlapEvents(false);
	}
}

void AMeleeAICharacter::StopMouv()
{
	if ( CanCombo == true)
	{
		if (AAIController* AIController = Cast<AAIController>(Controller))
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("StopMouv = true"));
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
				UE_LOG(LogTemp, Warning, TEXT("StopMouv = false"));
				BlackboardComp->SetValueAsBool("StopMouv", false);
			}
		}
	}

}

void AMeleeAICharacter::OnCompHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UnderTheFct"));
	if(CanCombo == true)
	{
		if(AGreenOneCharacter* CurrentPlayerRef = Cast<AGreenOneCharacter>(OtherActor))
		{
			
			UE_LOG(LogTemp, Warning, TEXT("2eme hit"));
			UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *CurrentPlayerRef->GetFName().ToString());
			IEntityGame::Execute_EntityTakeDamage(CurrentPlayerRef, Damage, SweepResult.BoneName, this);
			EndCollision();
		}
	}
	if(AGreenOneCharacter* CurrentPlayerRef = Cast<AGreenOneCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("1eme hit"));
		UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *CurrentPlayerRef->GetFName().ToString());
		IEntityGame::Execute_EntityTakeDamage(CurrentPlayerRef, Damage, SweepResult.BoneName, this);
		EndCollision();
		CanCombo = true;
	}

}




