// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_GreenOne.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GreenOne/Core/SaveGame/SG_GreenOne.h"
#include "GreenOne/Core/Audio/SG_AudioSettings.h"
#include "Engine/LevelStreaming.h"
#include "GreenOne/Widget/W_LoadingScreen.h"
#include "MoviePlayer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UGI_GreenOne::UGI_GreenOne() : UGameInstance()
{
	SaveClass = USG_GreenOne::StaticClass();
}

void UGI_GreenOne::Init()
{
	Super::Init();
	LoadSave();
	LoadAudioSave();
	FTimerHandle AudioHandle;
	GetWorld()->GetTimerManager().SetTimer(AudioHandle, this, &UGI_GreenOne::ApplyAudioSettings, 0.1f, false);
	//FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UGI_GreenOne::BeginLoadingScreen);
}

void UGI_GreenOne::LoadOneLevel(const FName LevelToLoad, UObject* TargetRef, const FName CallFunction, const bool ShouldUnload)
{
	DisplayLoadingScreen();
	ObjectTargetRef = TargetRef;
	CallFunctionRef = CallFunction;
	bShouldUnload = ShouldUnload;

	FLatentActionInfo LatInfo;
	LatInfo.CallbackTarget = this;
	if (ShouldUnload)
	{
		for (ULevelStreaming* CurrentLevel : GetWorld()->GetStreamingLevels())
		{
			if (!CurrentLevel)
			{
				UE_LOG(LogTemp, Warning, TEXT("Wesh Vide, just in case le level que t'as voulu check est vide bref."));
				continue;
			}
		if (CurrentLevel->IsLevelLoaded() && CurrentLevel->IsLevelVisible())
		{
			FName TargetMapName = CurrentLevel->GetWorldAssetPackageFName();
			FString CurrentMapStringName;
			TargetMapName.ToString(CurrentMapStringName);
			if (CurrentMapStringName.Contains("Level"))
			{
				LevelToUnload = TargetMapName;
				break;
			}
			UE_LOG(LogTemp, Warning, TEXT("Current Lvl : %s"), *LevelToUnload.ToString());
		}
		}
		LatInfo.ExecutionFunction = FName("UnloadMap");
	}
	else
	{
		LatInfo.ExecutionFunction = FName("CallFunctionAndRemoveLoading");
	}
	LatInfo.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), LevelToLoad, true, false, LatInfo);
}

void UGI_GreenOne::BeginLoadingScreen(const FString& MapName)
{
	if (MapMenuRef == nullptr)
	{
		return;
	}
	if (MapName == MapMenuRef->GetFName().ToString())
	{
		return;
	}
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass);
	if (LoadingWidget)
	{
		FLoadingScreenAttributes LoadingScreen;

		TSharedRef<SWidget> LoadingScreenWidget = SNullWidget::NullWidget;

		LoadingScreenWidget = LoadingWidget->TakeWidget();
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 1.5f;
		LoadingScreen.bMoviesAreSkippable = false;

		LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget;
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UGI_GreenOne::UnloadMap()
{
	if (ObjectTargetRef != nullptr)
	{
		UFunction* Function = ObjectTargetRef->FindFunction(CallFunctionRef);
		ObjectTargetRef->ProcessEvent(Function, nullptr);
	}
	if (bShouldUnload)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("RemoveLoadingScreen");
		LatentInfo.Linkage = 0;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), LevelToUnload, LatentInfo, false);
	}
}

void UGI_GreenOne::CallFunctionAndRemoveLoading()
{
	FTimerHandle RemoveLoadHandle;
	GetWorld()->GetTimerManager().SetTimer(RemoveLoadHandle, [&](){ RemoveLoadingScreen(); }, 1.5f, false);
	if (ObjectTargetRef != nullptr)
	{
		UFunction* Function = ObjectTargetRef->FindFunction(CallFunctionRef);
		ObjectTargetRef->ProcessEvent(Function, nullptr);
	}
}

#pragma region LoadingScreen

void UGI_GreenOne::DisplayLoadingScreen()
{
	if (!LoadingScreenClass)
	{
		return;
	}
	CurrentLoadingScreen = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass);
	if (IsValid(CurrentLoadingScreen))
	{
		CurrentLoadingScreen->AddToViewport();
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetWorld()->GetFirstPlayerController(), CurrentLoadingScreen, EMouseLockMode::DoNotLock, true);
	}
}

void UGI_GreenOne::RemoveLoadingScreen()
{
	if (CurrentLoadingScreen == nullptr)
	{
		return;
	}
	if (IsValid(CurrentLoadingScreen))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
		if (UW_LoadingScreen* CurrentLScreen = Cast<UW_LoadingScreen>(CurrentLoadingScreen))
		{
			CurrentLScreen->RemoveLoading();
		}
	}
}

#pragma endregion

#pragma region Save

void UGI_GreenOne::SaveGame()
{
	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error lors du save la variable currentSave est vide, Create une save avant"));	
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
	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Error, TEXT("La creation du save � fail."));
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
	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Error, TEXT("La save c'est mal passer il y un pointeur vide ici"));
		return;
	}
	CurrentSave->PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	CurrentSave->PlayerRotation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorRotation();

	for (ULevelStreaming* CurrentLevel : GetWorld()->GetStreamingLevels())
	{
		if (!CurrentLevel)
		{
			UE_LOG(LogTemp, Warning, TEXT("Wesh Vide, just in case le level que t'as voulu check est vide bref."));
			continue;
		}
		if (CurrentLevel->IsLevelLoaded() && CurrentLevel->IsLevelVisible())
		{
			FName TargetMapName = CurrentLevel->GetWorldAssetPackageFName();
			FString CurrentMapStringName;
			TargetMapName.ToString(CurrentMapStringName);
			if (CurrentMapStringName.Contains("Level"))
			{
				CurrentSave->MapName = TargetMapName;
				break;
			}
			UE_LOG(LogTemp, Warning, TEXT("Current Lvl : %s"), *CurrentSave->MapName.ToString());
		}
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Apply Save."));
	if (!CurrentSave) { return; }

	LoadOneLevel(CurrentSave->MapName, this, FName("ApplyLocation"));
}

void UGI_GreenOne::DisplaySaveScreen()
{
	if (CurrenSaveScreen == nullptr)
	{
		CurrenSaveScreen = CreateWidget<UUserWidget>(GetWorld(), SaveScreenClass);
		CurrenSaveScreen->AddToViewport();
	}
	else if (IsValid(CurrenSaveScreen))
	{
		CurrenSaveScreen->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	FTimerHandle LoadingHandler;
	GetWorld()->GetTimerManager().SetTimer(LoadingHandler, this, &UGI_GreenOne::DeleteSaveScreen, SaveTime, false);
}

void UGI_GreenOne::DeleteSaveScreen()
{
	CurrenSaveScreen->SetVisibility(ESlateVisibility::Collapsed);
	return;
}

void UGI_GreenOne::ApplyLocation()
{
	//FTimerHandle SetLocatioHandle;
	//GetWorld()->GetTimerManager().SetTimer(SetLocatioHandle, [&]() {
		APawn* PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!PlayerRef)
		{
			return;
		}
		PlayerRef->SetActorLocation(CurrentSave->PlayerLocation);
		PlayerRef->SetActorRotation(CurrentSave->PlayerRotation);
	//}, 0.5f, false);
}

#pragma endregion

#pragma region Save Setting Audio

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
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, MasterSoundClass, value, 1.f, FadeInTime);
}

void UGI_GreenOne::SetMusicVolume(float value)
{
	if (AudioSettingsRef == nullptr)
	{
		CreateAudioSave();
	}
	else
	{
		AudioSettingsRef->MusicVolume = value;
	}
	if (!MusicSoundClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Il n'y a pas de MusicSoundClass assigner dans le game instance"))
			return;
	}
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
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SFXSoundClass, value, 1.f, FadeInTime);
}

void UGI_GreenOne::CreateAudioSave()
{
	AudioSettingsRef = Cast<USG_AudioSettings>(UGameplayStatics::CreateSaveGameObject(USG_AudioSettings::StaticClass()));
	if (AudioSettingsRef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("La creation du Sound Save � Fail!!!"));
	}
	UGameplayStatics::SaveGameToSlot(AudioSettingsRef, AudioSaveName, SaveIndex + 1);
}

void UGI_GreenOne::LoadAudioSave()
{
	if (UGameplayStatics::DoesSaveGameExist(AudioSaveName, SaveIndex + 1))
	{
		AudioSettingsRef = Cast<USG_AudioSettings>(UGameplayStatics::LoadGameFromSlot(AudioSaveName, SaveIndex + 1));
		if (AudioSettingsRef == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Le cast du get Sound Save � Fail!!!"));
		}
	}
	else
	{
		CreateAudioSave();
	}
}

void UGI_GreenOne::SavedAudioSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(AudioSaveName, SaveIndex + 1))
	{
		if (AudioSettingsRef != nullptr)
		{
			UGameplayStatics::SaveGameToSlot(AudioSettingsRef, AudioSaveName, SaveIndex + 1);
			return;
		}
	}
	//if (!)
	//{
	UE_LOG(LogTemp, Warning, TEXT("Attention save du sound la variable AudioSettingsRef nullptr"));
	//	UE_LOG(LogTemp, Error, TEXT("Save audio FAILD!!!"));
	//}
}

void UGI_GreenOne::ApplyAudioSettings()
{
	if (AudioSettingsRef == nullptr)
	{
		CreateAudioSave();
	}
	SetMusicVolume(AudioSettingsRef->MusicVolume);
	SetMasterVolume(AudioSettingsRef->MasterVolume);
	SetSFXVolume(AudioSettingsRef->SFXVolume);
}

#pragma endregion
