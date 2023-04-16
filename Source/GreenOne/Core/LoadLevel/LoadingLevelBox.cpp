// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingLevelBox.h"
#include "Components/BoxComponent.h"

// Sets default values
ALoadingLevelBox::ALoadingLevelBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CollisionSize = FVector(32.f);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(CollisionSize, true);

}

void ALoadingLevelBox::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadingLevelBox::OnComponentOverlap);
}

void ALoadingLevelBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ALoadingLevelBox, CollisionSize))
	{
		CollisionBox->SetBoxExtent(CollisionSize, true);
	}
}

void ALoadingLevelBox::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO Load un level ici. je pense.
}

