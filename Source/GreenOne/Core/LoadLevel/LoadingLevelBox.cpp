// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingLevelBox.h"
#include "Components/BoxComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GreenOne/Core/Instance/GI_GreenOne.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/LevelStreaming.h"

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

#if WITH_EDITOR

void ALoadingLevelBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ALoadingLevelBox, LevelToLoad))
	{
		LevelNameText->SetText(FText::FromString(LevelToLoad.GetAssetName()));
	}
}

#endif // WITH_EDITOR

void ALoadingLevelBox::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerRef = Cast<AGreenOneCharacter>(OtherActor);
	if (PlayerRef != nullptr)
	{
		UGI_GreenOne* GameInstanceRef = Cast<UGI_GreenOne>(GetWorld()->GetGameInstance());
		if (GameInstanceRef != nullptr)
		{
			if (!LevelToLoad.IsNull())
			{
				const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));
				UE_LOG(LogTemp, Warning, TEXT("Map to load %s"), *LevelName.ToString());
				GameInstanceRef->LoadOneLevel(LevelName, this, FName("TpPlayer"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Je sais pas pourquoi t'es vide"));
			}
		}
	}
}

void ALoadingLevelBox::TpPlayer()
{
	if (UGI_GreenOne* GameInstanceRef = Cast<UGI_GreenOne>(GetWorld()->GetGameInstance()))
	{
		if (!PlayerStartRef.IsNull())
		{
			if (AActor* TargetLocation = Cast<AActor>(PlayerStartRef.LoadSynchronous()))
			{
				FTimerHandle TimerTP;
				GetWorld()->GetTimerManager().SetTimer(TimerTP, [=](){PlayerRef->SetActorLocation(TargetLocation->GetActorLocation()); }, 0.01f, false);
				UE_LOG(LogTemp, Warning, TEXT("HHAHAHAHAHA ça n'a pas marcher."));
			}
		}
		FTimerHandle LoadHandle;
		GetWorld()->GetTimerManager().SetTimer(LoadHandle, [=](){ GameInstanceRef->RemoveLoadingScreen(); }, 2.f, false);
	}
}

