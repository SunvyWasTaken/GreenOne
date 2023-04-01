#include "AIProjectil.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "GreenOne/AI/EnnemySpawner.h"

// Définit les valeurs par défaut
AAIProjectil::AAIProjectil()
{
	// Définissez ce actor pour appeler Tick() à chaque frame. Vous pouvez désactiver cette option pour améliorer les performances si vous n’en avez pas besoin.
	PrimaryActorTick.bCanEverTick = false;

	ProjectilComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectilMovement"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	Particules = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particules"));
	Particules->SetupAttachment(RootComponent);
	BoxComponent->bHiddenInGame = !bDebug;
	ProjectilComp->InitialSpeed = ProjectilSpeed;

}

// Appelé au début du jeu ou au moment de l'apparition de l'animal.
void AAIProjectil::BeginPlay()
{
	Super::BeginPlay();

	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAIProjectil::OnOverlap);
	}
}

void AAIProjectil::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AAIProjectil, bDebug))
	{
		BoxComponent->bHiddenInGame = !bDebug;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AAIProjectil, ProjectilSpeed))
	{
		ProjectilComp->InitialSpeed = ProjectilSpeed;
	}
}

void AAIProjectil::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning, TEXT("Projectile Collide with : %s"), *OtherActor->GetFName().ToString());
	if (!OtherActor)
	{
		return;
	}
	if (OtherActor == Owner || Cast<AEnnemySpawner>(OtherActor))
	{
		return;
	}
	if (OtherActor->Implements<UEntityGame>())
	{
		IEntityGame::Execute_EntityTakeDamage(OtherActor, ProjectilDamage, SweepResult.BoneName, Owner);
	}
	Destroy();
}

