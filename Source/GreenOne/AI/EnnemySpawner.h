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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif

public:

	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollisionActivation;

	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereCollisionDesactivation;

	UFUNCTION()
		void OnComponentActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void RemoveEntityFromList(class ABaseEnnemy* entity);

/************************************************************************/
/* Spawner Property														*/
/************************************************************************/

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store a reference to a subclass of the ABaseEnnemy class, which can then be used to spawn an instance of the subclass.
		TSubclassOf<class ABaseEnnemy> EnnemyToSpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store the number of enemies that can appear on the screen at the same time.
		int NbrEnnemySameTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store the delay between each spawn of an object.
		float DelayEachSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		// Spawner range detection in cm.
		float RangeDetection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store a float value that indicates whether range detection is disabled or not.
		float RangeDetectionDisable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store the number of enemies that will be spawned in the game.
		int NbrSpawnEnnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom")
		//This variable is used to store the minimum number of enemies that can be spawned in a game.
		int MinNbrSpawnEnnemy;

private:

	FTimerHandle SpawnHandler;

	void TriggerSpawnEntity();

	/// <summary>
	/// Spawns an entity in the game world.
	/// </summary>
	void SpawnEntity();

	// for a intern cooldown
	void SpawnTick(float deltaseconds);

	bool bCanSpawnEnnemy;

	float CurrentTimerRemaing;

	TArray<class ABaseEnnemy*> EntityList;

	AActor* PlayerRef;

	void SetPlayerRefToEntitys(AActor* ref);

};
