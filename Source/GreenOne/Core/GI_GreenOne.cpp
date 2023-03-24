// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_GreenOne.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SG_GreenOne.h"

void UGI_GreenOne::Init()
{
	Super::Init();
}

void UGI_GreenOne::SaveGame()
{
	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Error, TEXT("Error lors du save la variable currentSave est vide, Create une save avant"));
		return;
	}
	DisplaySaveScreen();
	UpdateSaveData();
	if (UGameplayStatics::DoesSaveGameExist(SaveName, SaveIndex))
	{
		UGameplayStatics::SaveGameToSlot(CurrentSave, SaveName, SaveIndex);
	}
	else
	{
		CurrentSave = CreateSave();
	}
}

bool UGI_GreenOne::LoadSave()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveName, SaveIndex))
	{
		CurrentSave = Cast<USG_GreenOne>(UGameplayStatics::LoadGameFromSlot(SaveName, SaveIndex));
	}
	else
	{
		CurrentSave = CreateSave();
	}
	if(!CurrentSave)
	{
		UE_LOG(LogTemp, Error, TEXT("La creation du save à fail."));
		return false;
	}
	return true;
}

void UGI_GreenOne::DeleteSave()
{
	if (UGameplayStatics::DeleteGameInSlot(SaveName, SaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Delete de la sauvegarde reussi."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Delete de la sauvegarde rate."));
	}
}

void UGI_GreenOne::UpdateSaveData()
{
	if (!CurrentSave) { return; }
	CurrentSave->PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
}

USG_GreenOne* UGI_GreenOne::CreateSave()
{
	if (!SaveClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Il n'y a pas de class save selectionner. Attention au crash"));
		return nullptr;
	}
	USG_GreenOne* Save = Cast<USG_GreenOne>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	UGameplayStatics::SaveGameToSlot(Save, SaveName, SaveIndex);
	return Save;
}

void UGI_GreenOne::ApplySaveData()
{
	if (!CurrentSave) { return; }
	APawn* PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerRef)
	{
		return;
	}
	PlayerRef->SetActorLocation(CurrentSave->PlayerLocation);
}

void UGI_GreenOne::DisplaySaveScreen()
{
	if (!LoadingScreenClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Class du widget loading vide"));
		return;
	}
	CurrenSaveScreen = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass);
	CurrenSaveScreen->AddToViewport();
	FTimerHandle LoadingHandler;
	GetWorld()->GetTimerManager().SetTimer(LoadingHandler, this, &UGI_GreenOne::DeleteSaveScreen, SaveTime, false);
}

void UGI_GreenOne::DeleteSaveScreen()
{
	CurrenSaveScreen->RemoveFromParent();
	CurrenSaveScreen = nullptr;
}
