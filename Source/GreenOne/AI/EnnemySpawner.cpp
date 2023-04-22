#include "EnnemySpawner.h"
#include "BaseEnnemy.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Définit les valeurs par défaut
AEnnemySpawner::AEnnemySpawner()
{
	// Définissez ce actor pour appeler Tick() à chaque frame. Vous pouvez désactiver cette option pour améliorer les performances si vous n’en avez pas besoin.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;

	TriggerArena = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box Arena"));
	TriggerArena->SetupAttachment(Root);

	/************************************************************************/
	/* WALL - E																*/
	/************************************************************************/
	WallsComponents = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls Comp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshClass(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshClass.Object != nullptr)
	{
		WallsComponents->SetStaticMesh(CubeMeshClass.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WallMaterialObject(TEXT("/Game/GreenOne/Materials/Instance/MI_FieldForce"));
	if (WallMaterialObject.Object != nullptr)
	{
		WallsComponents->SetMaterial(0, WallMaterialObject.Object);
	}
	WallsComponents->SetupAttachment(Root);
	WallsComponents->SetHiddenInGame(true, true);
	WallsComponents->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	/************************************************************************/
	/* Spawn Points															*/
	/************************************************************************/

	SpawnPoints = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SpawnPoints"));
	SpawnPoints->SetupAttachment(Root);
	SpawnPoints->SetHiddenInGame(true, true);
	SpawnPoints->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SpawnPointMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (SpawnPointMesh.Object != nullptr)
	{
		SpawnPoints->SetStaticMesh(SpawnPointMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SpawnPointMat(TEXT("/Game/GreenOne/VFX/Material/Instances/MI_Truc"));
	if (SpawnPointMat.Object != nullptr)
	{
		SpawnPoints->SetMaterial(0, SpawnPointMat.Object);
	}

}

// Appelé au début du jeu ou au moment de l'apparition de l'animal.
void AEnnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	TriggerArena->OnComponentBeginOverlap.AddDynamic(this, &AEnnemySpawner::OnComponentActivate);
	//PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Appelé chaque frame
void AEnnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnTick(DeltaTime);
}

void AEnnemySpawner::OnComponentActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AGreenOneCharacter>(OtherActor))
	{
		PlayerRef = OtherActor;
		TriggerSpawnEntity();
		WallsComponents->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WallsComponents->SetHiddenInGame(false, true);
		if (!bShouldKillAllEnnemys)
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnHandler, this, &AEnnemySpawner::TriggerSpawnEntity, DelayEachWave, true);
		}
		TriggerArena->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//SetPlayerRefToEntitys(PlayerRef);
	}
}

void AEnnemySpawner::RemoveEntityFromList(ABaseEnnemy* entity)
{
	EntityList.Remove(entity);
	if (bShouldKillAllEnnemys)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Nbr List : %d"), EntityList.Num());
		if (EntityList.Num() == 0)
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnHandler, this, &AEnnemySpawner::TriggerSpawnEntity, DelayEachWave, false);
			Victory();
		}
	}
}

/// <summary>
/// Triggers the spawning of an enemy entity with a delay between each spawn.
/// </summary>
void AEnnemySpawner::TriggerSpawnEntity()
{
	--NbrWave;
	for (int i = 0; i < NbrEnnemyPerWave; ++i )
	{
		SpawnEntity();
	}
}

void AEnnemySpawner::SpawnEntity()
{
	// Rewritten
	if (EnnemyToSpawnClass.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("La liste d'ennemie à spawn est vide xD"));
		return;
	}
	// Set spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	int Index = UKismetMathLibrary::RandomBoolWithWeight(EnnemyRatio/100.f);

	TSubclassOf<ABaseEnnemy> SelectEnnemyToSpawn = EnnemyToSpawnClass[Index];

	if (SelectEnnemyToSpawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("L'array est vide mec je sais pas comment t'as fait mais bref."))
		return;
	}

	// Spawn the actor
	ABaseEnnemy* CurrentSpawnEnnemy = GetWorld()->SpawnActor<ABaseEnnemy>(SelectEnnemyToSpawn, GetSpawnLocation(), GetActorRotation(), SpawnParams);

	// Add the spawned actor to the list
	if (CurrentSpawnEnnemy != nullptr)
	{
		EntityList.Add(CurrentSpawnEnnemy);
		if (PlayerRef != nullptr)
		{
			CurrentSpawnEnnemy->SetPlayerRef(PlayerRef);
			CurrentSpawnEnnemy->SpawnerRef = this;
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

FVector AEnnemySpawner::GetSpawnLocation()
{
	FTransform TargetLocation;
	SpawnPoints->GetInstanceTransform(CurrentSpawnInt, TargetLocation, true);
	if(SpawnPoints->GetInstanceCount() - 1 <= CurrentSpawnInt)
		{ CurrentSpawnInt = 0; }
	else
		{ ++CurrentSpawnInt; }
	return TargetLocation.GetLocation();
}

void AEnnemySpawner::Victory()
{
	if (NbrWave <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnHandler);
		WallsComponents->SetHiddenInGame(true, true);
		WallsComponents->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnnemySpawner::SetPlayerRefToEntitys(AActor* ref)
{
	for (ABaseEnnemy* Entity : EntityList)
	{
		Entity->SetPlayerRef(ref);
	}
}
