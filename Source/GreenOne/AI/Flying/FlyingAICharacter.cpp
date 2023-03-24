// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingAICharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GreenOne/Gameplay/EntityGame.h"

// Sets default values
AFlyingAICharacter::AFlyingAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlyingAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlyingAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickCooldown(DeltaTime);
}

// Called to bind functionality to input
void AFlyingAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFlyingAICharacter::Shoot()
{
	//GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &AFlyingAICharacter::TimerShoot, ShootRate, true);
	if (!IsInCooldown)
	{
		TimerShoot();
	}
}

void AFlyingAICharacter::ActiveCooldown()
{
	IsInCooldown = true;
	TimeRemainingForShoot = 1/ShootRate;
}

void AFlyingAICharacter::TickCooldown(float DeltaSeconds)
{
	if (IsInCooldown)
	{
		TimeRemainingForShoot -= DeltaSeconds;
		if (TimeRemainingForShoot <= 0.f)
		{
			IsInCooldown = false;
		}
	}
}

void AFlyingAICharacter::TimerShoot()
{
	FHitResult Outhit;
	
	// TODO à changer ici parce que le raycast touche l'ia Volante et c'est pas bon puis changer aussi la distance de tire parce que la elle est fixe Bref magic number toi meme tu sais.
	if (GetWorld()->LineTraceSingleByChannel(Outhit, GetActorLocation() + GetActorForwardVector() * 50, GetActorLocation() + (GetActorForwardVector() * ShootRange), ECC_Camera))
	{
		// Check si l'actor hit est vide ou pas.
		if (!Outhit.GetActor())
		{
			//UE_LOG(LogTemp, Error, TEXT("Actor Nulle ne rien faire parce que sinon ça crash xD."));
			return;
		}
		else if (Outhit.GetActor()->Implements<UEntityGame>())
		{
			ActiveCooldown();
			IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage);
		}
		//UE_LOG(LogTemp, Warning, TEXT("Touch : %s"), *Outhit.GetActor()->GetFName().ToString());
	}
}

