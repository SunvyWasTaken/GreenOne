#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIProjectil.generated.h"

UCLASS()
class GREENONE_API AAIProjectil : public AActor
{
	GENERATED_BODY()
	
public:
	// Définit les valeurs par défaut des propriétés de ce actor
	AAIProjectil();

protected:
	// Appelé au début du jeu ou au moment de l'apparition de l'animal.
	virtual void BeginPlay() override;

	virtual void Tick(float Deltatime) override;

public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "DrawDebug"), Category = "Custom|Debug")
	bool bDebug = false;

	#pragma region Magnetize
public:

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Rotation speed"), Category = "Custom|Magnetize")
	float RotationSpeed = 1.f;

	/**
	 * Distance minimum avant d'arrêter le magnétize.
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Minimum Distance magnetize"), Category = "Custom|Magnetize")
	float MinDistance = 100.f;

private:

	bool bMagnetize = true;

#pragma endregion

public:

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Vitesse du projectile", ClampMin = 0), Category = "Custom|Property")
	float ProjectilSpeed = 1200.f;

	// This property is useless now cause it is link to the Ai property
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Custom|Property")
	float ProjectilDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	class UProjectileMovementComponent* ProjectilComp;

	UPROPERTY(EditAnywhere, Category = "Custom")
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Custom")
	class UNiagaraComponent* Particules;

private:

UFUNCTION()
void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
