// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_GreenOne.generated.h"


class USG_GreenOne;

/**
 *
 */
UCLASS()
class GREENONE_API UGI_GreenOne : public UGameInstance
{
	GENERATED_BODY()

public:
	
	virtual void Init();

#pragma region Save

	/**
	 * Sauvegarde la current save.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void SaveGame();

	/**
	 * Load the save game mettre le nom de la save à load
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		bool LoadSave();

	/**
	 * Delete la save à l'espace donner
	 * TODO je sais pas si je dois vider la sauvegarde ou pas.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void DeleteSave();

	/**
	 * Name of the save.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Save")
		FString SaveName = "GreenOneSaveData";

	/**
	 * class du fichier sauvegarde à mettre absolument.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Save")
		TSubclassOf<class USaveGame> SaveClass;

	/**
	 * Class du widget à afficher lors de la sauvegarde.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Loading Screen")
		TSubclassOf<UUserWidget> LoadingScreenClass;

	/**
	 * Temps que le save game sera afficher
	 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0), Category = "Custom|Loading Screen")
		float SaveTime = 2.f;

protected:

	/**
	 * Save actuel.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Save")
		USG_GreenOne* CurrentSave;

	/**
	 * Ne pas utiliser cette fonction seul
	 * Recupe toutes les donnees a sauvgarder.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void UpdateSaveData();

	/**
	 * Fonction à ne pas utiliser seul utiliser juste les fonctions
	 * * Save Game
	 * * Load Save
	 * * Delete Save
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		USG_GreenOne* CreateSave();

	/**
	 * Index de la sauvgarde.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Save")
		int SaveIndex = 0;

	/// <summary>
	/// Applies the save data to the game.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void ApplySaveData();

private:

	class UUserWidget* CurrenSaveScreen;

	/// <summary>
	/// Displays the save screen.
	/// </summary>
	void DisplaySaveScreen();

	/// <summary>
	/// This method deletes the save screen.
	/// </summary>
	void DeleteSaveScreen();

#pragma endregion

#pragma region SaveSettingAudio

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
	float GetMasterVolume();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
	float GetMusicVolume();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audio")
	float GetSFXVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMasterVolume(float value);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMusicVolume(float value);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float value);

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f), Category = "Audio")
	float FadeInTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundMix* SoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* MusicSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundClass* SFXSoundClass;

private:

	void CreateAudioSave();
	
	void LoadAudioSave();

	void SavedAudioSettings();

	void ApplyAudioSettings();

	FString AudioSaveName = "AudioSettings";

	class USG_AudioSettings* AudioSettingsRef;

#pragma endregion

};
