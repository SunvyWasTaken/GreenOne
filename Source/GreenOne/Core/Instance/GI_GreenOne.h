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

	UGI_GreenOne();

	virtual void Init();

#pragma region LoadLevel

	UFUNCTION(BlueprintCallable)
		void LoadOneLevel(const FName LevelToLoad, UObject* TargetRef = nullptr, const FName CallFunction = "", const bool ShouldUnload = false);

	UFUNCTION()
		void BeginLoadingScreen(const FString& MapName);

private:

	UFUNCTION()
		void UnloadMap();

	UFUNCTION()
		void CallFunctionAndRemoveLoading();

	bool bShouldUnload;

	FName LevelToUnload;

	FName CallFunctionRef;

	UObject* ObjectTargetRef;

#pragma region Loading Screen

public:

	UPROPERTY(EditDefaultsOnly, Category = "Loading Screen")
		TSubclassOf<class UUserWidget> LoadingScreenClass;

	UPROPERTY(EditAnywhere, Category = "Reference")
		TSoftObjectPtr<class UWorld> MapMenuRef;


	/**
	* Display the loading screen for the selected map when it begins to load.
	* Cannot also use anywhere because this part is managed automatically.
	*/
	UFUNCTION(BlueprintCallable)
		void DisplayLoadingScreen();

	/**
	 * Remove the current loading indicator from the screen.
	 * Do not use it anywhere; it will be automatically called at the end of the loading process.
	 */
	UFUNCTION(BlueprintCallable)
		void RemoveLoadingScreen();

private:

	UPROPERTY()
	class UUserWidget* CurrentLoadingScreen;

#pragma endregion

#pragma region Save

public:

	/**
	 * Sauvegarde la current save.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void SaveGame();

	/**
	 * Load the save game mettre le nom de la save � load
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		bool LoadSave();

	/**
	 * Delete la save � l'espace donner
	 * TODO je sais pas si je dois vider la sauvegarde ou pas.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void DeleteSave();

	/// <summary>
	/// Applies the save data to the game.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Custom|Save")
		void ApplySaveData();

	/**
	 * Name of the save.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Save")
		FString SaveName = "GreenOneSaveData";

	/**
	 * class du fichier sauvegarde � mettre absolument.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Save")
		TSubclassOf<class USaveGame> SaveClass;

	/**
	 * Class du widget � afficher lors de la sauvegarde.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Loading Screen")
		TSubclassOf<UUserWidget> SaveScreenClass;

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
	 * Fonction � ne pas utiliser seul utiliser juste les fonctions
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

private:

	UPROPERTY()
	class UUserWidget* CurrenSaveScreen;

	/// <summary>
	/// Displays the save screen.
	/// </summary>
	void DisplaySaveScreen();

	/// <summary>	
	/// This method deletes the save screen.
	/// </summary>
	void DeleteSaveScreen();

	// Vrai function qui applique toutes les valeurs.
	UFUNCTION()
		void ApplyLocation();

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

	UFUNCTION(BlueprintCallable, Category = "Audio")
		void SavedAudioSettings();

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		FString AudioSaveName = "CustomAudioSettings";

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

	UFUNCTION()
		void CreateAudioSave();

	UFUNCTION()
		void LoadAudioSave();

	UFUNCTION()
		void ApplyAudioSettings();

	UPROPERTY()
	class USG_AudioSettings* AudioSettingsRef;

#pragma endregion

};
