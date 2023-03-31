// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_GreenOne.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SG_GreenOne.h"
#include "SG_AudioSettings.h"

void UGI_GreenOne::Init()
{
	Super::Init();
	if (LoadSave())
	{
		FTimerHandle SaveHandler;
		GetWorld()->GetTimerManager().SetTimer(SaveHandler, [&](){ApplySaveData();}, 0.2f, false);
	}
	LoadAudioSave();
}

#pragma region Save

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
		CurrentSave->bIsFirstTime = false;
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
		UE_LOG(LogTemp, Warning, TEXT("Delete de la sauvegarde failed."));
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
	if (CurrentSave->bIsFirstTime) { return; }
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

#pragma endregion

float UGI_GreenOne::GetMasterVolume()
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	return AudioSettingsRef->MasterVolume;
}

float UGI_GreenOne::GetMusicVolume()
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	return AudioSettingsRef->MusicVolume;
}

float UGI_GreenOne::GetSFXVolume()
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	return AudioSettingsRef->SFXVolume;
}

void UGI_GreenOne::SetMasterVolume(float value)
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	AudioSettingsRef->MasterVolume = value;
	if (!MasterSoundClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Il n'y a pas de MasterSoundClass assigner dans le game instance"))
		return;
	}
	SavedAudioSettings();
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, MasterSoundClass, value, 1.f, FadeInTime);
}

void UGI_GreenOne::SetMusicVolume(float value)
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	AudioSettingsRef->MusicVolume = value;
	if (!MusicSoundClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Il n'y a pas de MusicSoundClass assigner dans le game instance"))
			return;
	}
	SavedAudioSettings();
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, MusicSoundClass, value, 1.f, FadeInTime);
}

void UGI_GreenOne::SetSFXVolume(float value)
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	AudioSettingsRef->SFXVolume = value;
	if (!SFXSoundClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Il n'y a pas de SFXSoundClass assigner dans le game instance"))
			return;
	}
	SavedAudioSettings();
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SFXSoundClass, value, 1.f, FadeInTime);
}

void UGI_GreenOne::CreateAudioSave()
{
	AudioSettingsRef = Cast<USG_AudioSettings>(UGameplayStatics::CreateSaveGameObject(USG_AudioSettings::StaticClass()));
	UGameplayStatics::SaveGameToSlot(AudioSettingsRef, AudioSaveName, SaveIndex);
}

void UGI_GreenOne::LoadAudioSave()
{
	if (UGameplayStatics::DoesSaveGameExist(AudioSaveName, SaveIndex))
	{
		AudioSettingsRef = Cast<USG_AudioSettings>(UGameplayStatics::LoadGameFromSlot(AudioSaveName, SaveIndex));
	}
	else
	{
		CreateAudioSave();
	}
	ApplyAudioSettings();
}

void UGI_GreenOne::SavedAudioSettings()
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	UGameplayStatics::SaveGameToSlot(AudioSettingsRef, AudioSaveName, SaveIndex);
}

void UGI_GreenOne::ApplyAudioSettings()
{
	if (!AudioSettingsRef)
	{
		CreateAudioSave();
	}
	SetMusicVolume(AudioSettingsRef->MusicVolume);
	SetMasterVolume(AudioSettingsRef->MasterVolume);
	SetSFXVolume(AudioSettingsRef->SFXVolume);
}
