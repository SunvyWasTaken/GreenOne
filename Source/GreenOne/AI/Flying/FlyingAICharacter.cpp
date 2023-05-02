// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingAICharacter.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"	
#include "Engine/CollisionProfile.h"
#include "AIProjectil.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFlyingAICharacter::AFlyingAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentHeight = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CurrentHeight"));
	CurrentHeight->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundObject(TEXT("/Game/GreenOne/SFX/S_AvancerKamikazeCisailleur"));
	if (SoundObject.Object != NULL)
	{
		SoundClass = SoundObject.Object;
	}

	ExploRadius = 100.f;
}

// Called when the game starts or when spawned
void AFlyingAICharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxFlySpeed = MaxSpeed;
	}
	this->OnTakeDamage.AddDynamic(this, &AFlyingAICharacter::OnShinderu);

	UMaterialInstanceDynamic* InstanceMat = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	InstanceMat->SetScalarParameterValue(FName("EmissiveIntensity"), 2000);
}

void AFlyingAICharacter::DeadEntity()
{
	if(AudioWarning)
		AudioWarning->FadeOut(0.1f, 0.f, EAudioFaderCurve::Linear);
	Super::DeadEntity();
}

// Called every frame
void AFlyingAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickCooldown(DeltaTime);
	TickRotation(DeltaTime);
	CheckHeight(DeltaTime);
	CancelShoot();
}

void AFlyingAICharacter::CheckHeight(float Deltatime)
{
	FHitResult OuthitGround;
	FVector StartLocation = GetActorLocation() + (FVector::UpVector * -70.f);
	FVector EndLocation = StartLocation + ((FVector::UpVector * -1) * (MaxFlyHeight + 1.f));
	bool bIsHitSomething = GetWorld()->LineTraceSingleByChannel(OuthitGround, StartLocation, EndLocation, ECC_Visibility);
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, Deltatime);
	if (bIsHitSomething)
	{
		CurrentHHH = OuthitGround.Distance;
		if (OuthitGround.Distance < MinFlyHeight)
		{
			GetMovementComponent()->AddInputVector(FVector::UpVector);
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Goes Down"));
		GetMovementComponent()->AddInputVector(FVector::UpVector * -1);
	}
}

void AFlyingAICharacter::Shoot()
{
	//GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &AFlyingAICharacter::TimerShoot, ShootRate, true);
	if (!IsInCooldown)
	{
		bIsShooting = true;
		//TimerShoot();
	}
}

void AFlyingAICharacter::CancelShoot()
{
	if (!bIsShooting || !bCanCancelShoot)
	{
		return;
	}
	AActor* PlayerRef = GetWorld()->GetFirstPlayerController()->GetCharacter();
	if (PlayerRef != nullptr)
	{
		FVector PLayerLocation = PlayerRef->GetActorLocation();
		double DistanceBet = FVector::Dist(PLayerLocation, GetActorLocation());
		if (DistanceBet <= MinShootDistance || DistanceBet >= MaxShootDistance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cancel l'attaque!!!"));
			bIsShooting = false;
		}
	}
}

void AFlyingAICharacter::UpdateMaxSpeed(float NewSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Update Speed"));
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxFlySpeed = NewSpeed;
	}
}

void AFlyingAICharacter::SetRotationAxis(FVector2D TargetAxis)
{
	TargetRotationInput = TargetAxis;
}

//This function is used to perform self-destruction of the AI character.
void AFlyingAICharacter::SelfDestruction()
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
				IEntityGame::Execute_EntityTakeDamage(CurrentPlayerRef, Damage * RatioExploDmg, Outhit.BoneName, this);
				//Break out of the loop
				break;
			}
		}
	}
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExploRadius, 12, FColor::Red, true, 5.f);
#endif
	//Check if the ExplosionParticule is not null
	if (ExplosionParticule != nullptr)
	{
		//Spawn the ExplosionParticule at the actor's location
		UNiagaraComponent* CurrentExploParticule = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionParticule, GetActorLocation());
		CurrentExploParticule->SetVariableFloat("ExplosionRadius", ExploRadius);
	}
	//Call the DeadEntity function
	DeadEntity();
}

void AFlyingAICharacter::OnShinderu(float NbrDamage)
{
	if (IsAlreadyDead)
	{
		return;
	}
	else if (GetPercentHealth() <= ExploTreshold)
	{
		IsAlreadyDead = true;
		AudioWarning = UGameplayStatics::SpawnSoundAttached(SoundClass, RootComponent, FName(""), FVector::ZeroVector, EAttachLocation::SnapToTarget);
		AudioWarning->FadeIn(1.f, 1.f, 0.f, EAudioFaderCurve::Linear);

		SpawnWarning();
	}
}

void AFlyingAICharacter::SpawnWarning()
{
	UMaterialInstanceDynamic* InstanceMat = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	InstanceMat->SetScalarParameterValue(FName("EmissiveIntensity"), 200000);
	if (WarningExplosion != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning!!"));
		UNiagaraFunctionLibrary::SpawnSystemAttached(WarningExplosion, GetMesh(), FName("SocketShinderu"), FVector::ZeroVector, GetActorRotation(), FVector::OneVector, EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease);
	}
}

//This function sets the IsInCooldown boolean to true and sets the TimeRemainingForShoot to 1 divided by the ShootRate. 
void AFlyingAICharacter::ActiveCooldown()
{
	IsInCooldown = true; //Set IsInCooldown to true
	TimeRemainingForShoot = 1 / ShootRate; //Set TimeRemainingForShoot to 1 divided by the ShootRate
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
	bIsShooting = false;
	if (bUseTrace)
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

				//Check if the actor that was hit is the same as this actor
				if (Outhit.GetActor() == this)
				{
					//If so, return
					return;
				}
				//Check if the actor implements the UEntityGame interface
				else if (Outhit.GetActor()->Implements<UEntityGame>())
				{
					//If so, activate the cooldown and execute the EntityTakeDamage function
					ActiveCooldown();
					IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage * RatioDmgShoot, Outhit.BoneName, this);
				}
				//UE_LOG(LogTemp, Warning, TEXT("Touch : %s"), *Outhit.GetActor()->GetFName().ToString());
			}
		}
	}
	else
	{
		if (!ProjectileClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("La class Projectil de l'ennemie est vide!!"));
			return;
		}
		ActiveCooldown();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = this;
		AAIProjectil* CurrentBullet = GetWorld()->SpawnActor<AAIProjectil>(ProjectileClass, GetActorTransform(), SpawnParam);
		if (CurrentBullet)
		{
			UE_LOG(LogTemp, Warning, TEXT("Nbr Damage : %f"), Damage * RatioDmgShoot);
			CurrentBullet->ProjectilDamage = Damage * RatioDmgShoot;
		}
	}
}

void AFlyingAICharacter::TickRotation(float DeltaSeconds)
{
	CurrentRotationInput = UKismetMathLibrary::Vector2DInterpTo_Constant(CurrentRotationInput, TargetRotationInput, DeltaSeconds, RotationSpeed);
	//UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f2"), CurrentRotationInput.X, CurrentRotationInput.Y);
}

