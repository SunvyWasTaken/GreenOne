// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "InputActionValue.h"
#include "GreenOneCharacter.generated.h"

class UInputAction;
UCLASS(config=Game)
class AGreenOneCharacter : public ACharacter, public IEntityGame
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitEnnemy);

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackMeleeAction;

	UPROPERTY(EditAnywhere)
	class UAttackMelee* AttackMeleeComponent;
	
	void PlayerDead();

	virtual void Tick( float DeltaSeconds );

	void InputJump(const FInputActionValue& Value);
	
public:

	AGreenOneCharacter();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EntityTakeDamage(float damage, FName BoneNameHit, AActor* DamageSource = nullptr);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Player")
	float Health = 100.f;

	/**
	 * Return une valeur entre 0 et 1 correspondant au percentage de la vie du joueur.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealthPercent();

	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamage;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;

#pragma region Tire

	UPROPERTY(BlueprintAssignable)
	FOnHitEnnemy OnHitEnnemy;

	/**
	 * Give if the player is attacking or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Combat")
	bool IsAttacking();

	UFUNCTION(BlueprintCallable, Category = "Custom|Combat")
	void Shoot();

	UFUNCTION(BlueprintCallable, Category = "Custom|Combat")
	void StopShoot();

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0), Category = "Custom|Combat")
	UFUNCTION(BlueprintCallable)
	void AttackMelee();

	UPROPERTY(EditAnywhere)
	float DamagePlayer = 10.f;

	/**
	 * Cooldown entre chaque tire par d�fault c'est 1/3;
	 * c'est � dire 1 tire toutes les 3 secondes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0), Category = "Custom|Combat")
	float ShootCooldown;

	/**
	 * Distance que le tire va atteindre depuis l'avant du gun.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0, DisplayName = "Distance de Tire"), Category = "Custom|Combat")
	float ShootDistance = 400.f;

	/**
	 * Taux de dispertion du tire.
	 * Allant de 0 � 1.
	 * 0 quand il n'y a pas de bloom et 1 les tires fuse � 360� autour du joueur.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1, DisplayName = "Bloom du Tire"), Category = "Custom|Combat")
	float ShootBloom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Decal dot ref d'impact"), Category = "Custom|Combat")
	TSubclassOf<AActor> DotDecal;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Combat")
	float ShootCooldownRemaining;

private:

	bool CanShoot;

	FTimerHandle ShootHandler;

	void ShootRafale();

	void ShootTick(float deltatime);

	//void DotImpact();

#pragma endregion

private:

	bool bIsDead = false;

#pragma region Dash

public:

	// Dash dans la direction de l'input mouvement.
	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
	void Dash();

	// Distance du dash
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Vitesse du dash", ClampMin = 0), Category = "Custom|Dash")
	float DashDistance;

	// Le temps que va prendre le dash pour attendre �a destination.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps du dash", ClampMin = 0), Category = "Custom|Dash")
	float DashTime;

	// Temps que va prendre le dash � revenir apr�s utilisation.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps de recharge du Dash"), Category = "Custom|Dash")
	float DashCooldown;

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "IsDashing"), Category = "Custom|Dash")
	bool bIsDashing;

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Dash")
	bool bDashOnCooldown;

	/**
	 * Return the remaining time of the dash cooldown.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Keywords = "Cooldown|Dash"), Category = "Dash")
	float GetRemainingDashTime() { return CurrentDashCooldown; };

private:

	// Utiliser pour placer le player pendant le Dash
	void DashTick(float deltatime);

	void CooldownDash(float deltatime);

	FVector TargetDashLocation;

	FVector StartDashLocation;

	float CurrentDashAlpha;

	float CurrentDashCooldown;

#pragma endregion

#pragma region Pause

public:

	UFUNCTION(BlueprintCallable)
	void TogglePauseGame();

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Pause")
	TSubclassOf<UUserWidget> PauseWidgetClass;

private:

	class UUserWidget* PauseWidgetRef;

#pragma endregion

protected:

	void Move(const FInputActionValue& Value);

	float MaxHealth = 0;

	UPROPERTY(BlueprintReadWrite)
	bool IsAtk;

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

