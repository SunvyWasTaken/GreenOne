#include "EnnemySpawner.h"
#include "Components/SphereComponent.h"
#include "BaseEnnemy.h"

// Définit les valeurs par défaut
AEnnemySpawner::AEnnemySpawner()
{
	// Définissez ce actor pour appeler Tick() à chaque frame. Vous pouvez désactiver cette option pour améliorer les performances si vous n’en avez pas besoin.
	PrimaryActorTick.bCanEverTick = true;

	NbrSpawnEnnemy = 10;

	RangeDetection = 1000.f;

	SphereCollisionActivation = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	RootComponent = SphereCollisionActivation;

	SphereCollisionActivation->SetSphereRadius(RangeDetection);

}

// Appelé au début du jeu ou au moment de l'apparition de l'animal.
void AEnnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	SphereCollisionActivation->OnComponentBeginOverlap.AddDynamic(this, &AEnnemySpawner::OnComponentActivate);
}

// Appelé chaque frame
void AEnnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnTick(DeltaTime);
}

void AEnnemySpawner::OnComponentActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TriggerSpawnEntity();
}

void AEnnemySpawner::OnComponentDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnHandler);
}

void AEnnemySpawner::RemoveEntityFromList(ABaseEnnemy* entity)
{
	EntityList.Remove(entity);
}

/// <summary>
/// Triggers the spawning of an enemy entity with a delay between each spawn.
/// </summary>
void AEnnemySpawner::TriggerSpawnEntity()
{
	if (bCanSpawnEnnemy)
	{
		bCanSpawnEnnemy = false;
		CurrentTimerRemaing = DelayEachSpawn;
		GetWorld()->GetTimerManager().SetTimer(SpawnHandler, this, &AEnnemySpawner::SpawnEntity, DelayEachSpawn, true);
		SpawnEntity();
	}
}

void AEnnemySpawner::SpawnEntity()
{
	if(EntityList.Num() >= NbrEnnemySameTime)
		{ return; }
	// Rewritten
	if (EnnemyToSpawnClass != nullptr)
	{
		// Set spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn the actor
		ABaseEnnemy* CurrentSpawnEnnemy = GetWorld()->SpawnActor<ABaseEnnemy>(EnnemyToSpawnClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		// Add the spawned actor to the list
		if (CurrentSpawnEnnemy != nullptr)
		{
			EntityList.Add(CurrentSpawnEnnemy);
			CurrentSpawnEnnemy->ParentRef = this;
		}
	}
}

//This function is used to spawn enemies.
void AEnnemySpawner::SpawnTick(float deltaseconds)
{
	//Check if an enemy can be spawned.
	if (!bCanSpawnEnnemy)
	{
		//Decrement the timer.
		CurrentTimerRemaing -= deltaseconds;
		//Check if the timer has reached 0.
		if (CurrentTimerRemaing <= 0.f)
		{
			//Set the flag to true, allowing an enemy to be spawned.
			bCanSpawnEnnemy = true;
		}
	}
}

