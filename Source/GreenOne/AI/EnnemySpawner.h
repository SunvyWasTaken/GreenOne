#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnnemySpawner.generated.h"

UCLASS()
class GREENONE_API AEnnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Définit les valeurs par défaut des propriétés de ce actor
	AEnnemySpawner();

protected:
	// Appelé au début du jeu ou au moment de l'apparition de l'animal.
	virtual void BeginPlay() override;

public:
	// Appelé chaque frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditDefaultsOnly)
		class USceneComponent* Root;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* TriggerArena;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UInstancedStaticMeshComponent* WallsComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UInstancedStaticMeshComponent* SpawnPoints;;

	//UPROPERTY(EditAnywhere)
		//class USphereComponent* SphereCollisionDesactivation;

	//UFUNCTION()
		//void OnComponentDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void RemoveEntityFromList(class ABaseEnnemy* entity);

	/// <summary>
	/// Spawns an entity in the game world.
	/// </summary>
	UFUNCTION(BlueprintCallable)
		void SpawnEntity();

	UFUNCTION(BlueprintCallable)
		void SetPlayerRefToEntitys(AActor* ref);

private:

	// UFUNCTION()
	// 	void OnComponentActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	/************************************************************************/
	/* Spawner Property														*/
	/************************************************************************/

public:

	//This variable is used to store a reference to a subclass of the ABaseEnnemy class, which can then be used to spawn an instance of the subclass.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
		TArray<TSubclassOf<class ABaseEnnemy>> EnnemyToSpawnClass;

	// Enemy spawn ratio in the list above.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 100, UIMin = 0, UIMax = 100), Category = "Custom")
		float EnnemyRatio = 50.f;

	// Number of enemies per wave.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		int NbrEnnemyPerWave = 20;

	// Is it that the player has to kill all the enemies before the next wave launches?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Kill All"), Category = "Custom")
		bool bShouldKillAllEnnemys = true;

	// Time between each wave.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		float DelayEachWave = 10.f;

	// Number of waves the Spawner will perform.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		int NbrWave = 2;


private:

	FTimerHandle SpawnHandler;

	void TriggerSpawnEntity();

	// for a intern cooldown
	void SpawnTick(float deltaseconds);

	FVector GetSpawnLocation();

	// Function to call when victory.
	void Victory();

	int CurrentSpawnInt = 0;

	bool bCanSpawnEnnemy;

	float CurrentTimerRemaing;

	TArray<class ABaseEnnemy*> EntityList;

	AActor* PlayerRef;

};
