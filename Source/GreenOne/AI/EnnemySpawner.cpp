#include "EnnemySpawner.h"
#include "Components/SphereComponent.h"
#include "BaseEnnemy.h"
#include "GreenOne/GreenOneCharacter.h"

// Définit les valeurs par défaut
AEnnemySpawner::AEnnemySpawner()
{
	// Définissez ce actor pour appeler Tick() à chaque frame. Vous pouvez désactiver cette option pour améliorer les performances si vous n’en avez pas besoin.
	PrimaryActorTick.bCanEverTick = true;

	NbrSpawnEnnemy = 10;

	RangeDetection = 1000.f;
	RangeDetectionDisable = 1100.f;

	SphereCollisionActivation = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	RootComponent = SphereCollisionActivation;
	SphereCollisionDesactivation = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionDesactivation"));
	SphereCollisionDesactivation->SetupAttachment(RootComponent);

	SphereCollisionActivation->SetSphereRadius(RangeDetection);
	SphereCollisionDesactivation->SetSphereRadius(RangeDetectionDisable);
}

void AEnnemySpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Call parent implementation of this function first
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if the property that changed is RangeDetection
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AEnnemySpawner, RangeDetection))
	{
		// Update the sphere radius
		SphereCollisionActivation->SetSphereRadius(RangeDetection);
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AEnnemySpawner, RangeDetectionDisable))
	{
		SphereCollisionDesactivation->SetSphereRadius(RangeDetectionDisable);
	}
}


// Appelé au début du jeu ou au moment de l'apparition de l'animal.
void AEnnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	SphereCollisionActivation->OnComponentBeginOverlap.AddDynamic(this, &AEnnemySpawner::OnComponentActivate);
	SphereCollisionDesactivation->OnComponentEndOverlap.AddDynamic(this, &AEnnemySpawner::OnComponentDeactivate);
}

// Appelé chaque frame
void AEnnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnTick(DeltaTime);
}

void AEnnemySpawner::OnComponentActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerRef = Cast<AGreenOneCharacter>(OtherActor);
	if (PlayerRef)
	{
		FTimerHandle PlayerRefHandle;
		SetPlayerRefToEntitys(PlayerRef);
	}
	TriggerSpawnEntity();
}

void AEnnemySpawner::OnComponentDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AGreenOneCharacter>(OtherActor))
	{
		return;
	}
	PlayerRef = nullptr;
	SetPlayerRefToEntitys(PlayerRef);
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
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Spawn the actor
		ABaseEnnemy* CurrentSpawnEnnemy = GetWorld()->SpawnActor<ABaseEnnemy>(EnnemyToSpawnClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		// Add the spawned actor to the list
		if (CurrentSpawnEnnemy != nullptr)
		{
			EntityList.Add(CurrentSpawnEnnemy);
			CurrentSpawnEnnemy->ParentRef = this;
			if (PlayerRef)
			{
				CurrentSpawnEnnemy->SetPlayerRef(PlayerRef);
			}
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

void AEnnemySpawner::SetPlayerRefToEntitys(AActor* ref)
{
	for (ABaseEnnemy* Entity : EntityList)
	{
		Entity->SetPlayerRef(ref);
	}
}
