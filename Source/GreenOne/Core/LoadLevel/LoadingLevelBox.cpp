// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingLevelBox.h"
#include "Components/BoxComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GreenOne/Core/Instance/GI_GreenOne.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
ALoadingLevelBox::ALoadingLevelBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	LevelNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LevelNameText"));
	LevelNameText->SetupAttachment(RootComponent);

}

void ALoadingLevelBox::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadingLevelBox::OnComponentOverlap);
}

void ALoadingLevelBox::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO Load un level ici. je pense.
	if (Cast<AGreenOneCharacter>(OtherActor))
	{
		UGI_GreenOne* GameInstanceRef = Cast<UGI_GreenOne>(GetWorld()->GetGameInstance());
		if (GameInstanceRef != nullptr)
		{
			GameInstanceRef->LoadOneLevel(LevelToLoad);
			if (PlayerStartRef != nullptr)
			{
				OtherActor->SetActorLocation(PlayerStartRef->GetActorLocation());
			}
		}
	}
}

