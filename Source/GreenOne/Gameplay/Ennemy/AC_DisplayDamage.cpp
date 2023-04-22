// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_DisplayDamage.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UAC_DisplayDamage::UAC_DisplayDamage()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
	TextDamage = ATextRenderActor::StaticClass();
}


// Called when the game starts
void UAC_DisplayDamage::BeginPlay()
{
	Super::BeginPlay();

	if (ABaseEnnemy* OwnerRef = Cast<ABaseEnnemy>(GetOwner()))
	{
		OwnerRef->OnTakeDamage.AddDynamic(this, &UAC_DisplayDamage::DisplayDamage);
	}
}

void UAC_DisplayDamage::DisplayDamage(float NbrDamage)
{
	if (TextDamage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("T'as oublié de mettre le TextRender Dans t'as variable"));
		return;
	}
	ATextRenderActor* CurrentText = GetWorld()->SpawnActor<ATextRenderActor>(TextDamage, GetOwner()->GetActorTransform());
	CurrentText->InitialLifeSpan = 1.f;
	CurrentText->GetTextRender()->SetText(FText::FromString(FString::FromInt(NbrDamage)));
	AActor* PlayerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CurrentText->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(PlayerRef->GetActorLocation(), GetOwner()->GetActorLocation()));
}

