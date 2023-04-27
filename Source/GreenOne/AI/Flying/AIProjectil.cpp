#include "AIProjectil.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "GreenOne/AI/EnnemySpawner.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Définit les valeurs par défaut
AAIProjectil::AAIProjectil()
{
	// Définissez ce actor pour appeler Tick() à chaque frame. Vous pouvez désactiver cette option pour améliorer les performances si vous n’en avez pas besoin.
	PrimaryActorTick.bCanEverTick = true;

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

void AAIProjectil::Tick(float Deltatime)
{
	Super::Tick(Deltatime);
	if (!bMagnetize)
	{ return; }
	const FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	const float DistanceToPlayer = FVector::Distance(PlayerLocation, GetActorLocation());
	if (DistanceToPlayer <= MinDistance)
	{
		bMagnetize = false;
	}
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);
	SetActorRotation(UKismetMathLibrary::RInterpTo_Constant(GetActorRotation(), TargetRotation, Deltatime, RotationSpeed));
	ProjectilComp->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectilSpeed);
}

#if WITH_EDITOR

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

#endif

void AAIProjectil::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}
	if (OtherActor == Owner || Cast<AEnnemySpawner>(OtherActor) || Cast<AAIProjectil>(OtherActor))
	{
		return;
	}
	if (OtherActor->Implements<UEntityGame>())
	{
		IEntityGame::Execute_EntityTakeDamage(OtherActor, ProjectilDamage, SweepResult.BoneName, Owner);
	}
	Destroy();
}

