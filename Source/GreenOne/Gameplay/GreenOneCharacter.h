// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "InputActionValue.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerBase.h"
#include "GreenOne/Gameplay/Effects/Fertilizer/FertilizerFactory.h"
#include "GreenOneCharacter.generated.h"

class UInputAction;
UCLASS(config=Game)
class AGreenOneCharacter : public ACharacter, public IEntityGame
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRegen);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitEnnemy, class ABaseEnnemy*, CurrentTarget);

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
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackMeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EnableFertilizerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeFertilizerTypeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere)
	class UAttackMelee* AttackMeleeComponent;
	
	UFUNCTION(BlueprintCallable)
	void SetLastTouchLocation(FVector Location);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Mouvement")
	class UCustomCharacterMovementComponent* CustomCharacterMovementComponent;

	virtual void PostInitializeComponents() override;
	
public:

	AGreenOneCharacter(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EntityTakeDamage(float damage, FName BoneNameHit, AActor* DamageSource = nullptr);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Player")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Player")
	float JumpVelocity = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Player")
	float WalkSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Player")
	float ReverseSlow = 0.33333f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Player")
	float LateralSlow = 0.5f;

	UPROPERTY(BlueprintReadWrite)
	bool IsAtk;

	/**
	 * Return une valeur entre 0 et 1 correspondant au percentage de la vie du joueur.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealthPercent();

	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamage;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;

protected:

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Health");
	float MaxHealth = 100;

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

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

private:

	bool bIsDead = false;

	void PlayerDead();

	virtual void Tick(float DeltaSeconds);

	void InputJump(const FInputActionValue& Value);

	void EnableFertilizer();

	void ChangeFertilizerType();

	void Interact();

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool Invisible = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool Immortal = false;

	UFUNCTION()
	void Respawn();

	FVector LastTouchLocation;

public:

	/** Returns CustomCharacterMovementComponent subobject **/
	UFUNCTION(BlueprintCallable, Category = "Custom|Movement")
	FORCEINLINE class UCustomCharacterMovementComponent* GetCustomCharacterMovement() const { return CustomCharacterMovementComponent; }
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region Shoot

public:

	UPROPERTY(BlueprintAssignable)
	FOnHitEnnemy OnHitEnnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class USceneComponent* TargetMuzzle;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Important")
	FName SocketMuzzle;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Important")
	class UNiagaraSystem* ShootParticule;

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
	float DamagePlayer = 10.f;

	/**
	 * Cooldown entre chaque tire par default c'est 1/3;
	 * c'est a dire 1 tire toutes les 3 secondes.
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
	 * Allant de 0 a 1.
	 * 0 quand il n'y a pas de bloom et 1 les tires fuse a 360degres autour du joueur.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1, DisplayName = "Bloom du Tire"), Category = "Custom|Combat")
	float ShootBloom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Decal dot ref d'impact"), Category = "Custom|Combat")
	class UNiagaraSystem* ImpactParticules;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Combat")
	float ShootCooldownRemaining;

private:

	FVector LocationToAim;

	bool IsTouchSomething;

	bool CanShoot;

	FTimerHandle ShootHandler;

	void ShootRafale();

	void ShootTick(float deltatime);

	//void DotImpact();

#pragma endregion

#pragma region Mode
public:

	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void CanRegenerate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Player")
		bool IsCombatMode = false;


	UFUNCTION(BlueprintCallable)
		void Regenerate(float DeltaSeconds);

	UPROPERTY(BlueprintAssignable)
		FOnRegen OnRegen;

private:
	FTimerHandle TimerRegen;

	/** Valeur d'incrémentation du cooldown après chaque attaque */
	UPROPERTY(EditAnywhere, Category = "Custom|Player|RegeneateHealth", DisplayName = "Valeur de temps apres avoir ete en mode attack")
	float CoolDown = 5.f;

#pragma endregion 

#pragma region Pause

public:

	UFUNCTION(BlueprintCallable, Category = "Custom|Pause")
	void TogglePauseGame();

	UPROPERTY(EditDefaultsOnly, Category = "Custom|Pause")
	TSubclassOf<UUserWidget> PauseWidgetClass;

private:

	class UUserWidget* PauseWidgetRef;

#pragma endregion

#pragma region TurnCamera

private:
	UPROPERTY(EditDefaultsOnly, Category = "Custom|Camera")
	float Multiplicator;

	UFUNCTION()
	void TurnCamera();

#pragma endregion 

#pragma region HorizontalJump
	
	UPROPERTY(EditAnywhere, Category = "Custom|Jump|Horizontal", DisplayName = "Editer la rapidité du jump horizontal")
	bool bManualHorizontalVelocity = false;
	/** Default value of horizontal jump is the same that jump velocity */
	UPROPERTY(EditAnywhere, Category = "Custom|Jump|Horizontal", meta = (ForceUnits = "cm/s", EditCondition="bManualHorizontalVelocity"), DisplayName = "Rapidité du jump horizontal")
	float HorizontalJumpVelocity = 450.f;
	bool bHorizontalJump;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Jump|Horizontal", meta = (ForceUnits = "cm/s"), DisplayName = "Distance du jump horizontal")
	float MaxDistanceHorizontalJump = 450.f;
	float DistanceHorizontalJump;
	FVector TargetHorizontalJump = FVector::ZeroVector;

	FVector2D HorizontalJumpDirection = FVector2D::ZeroVector;
	float TargetDistance = 0;

	FVector CurrentLocation;

	UFUNCTION(BlueprintCallable, Category = "Custom|Jump|Horizontal", DisplayName = "Double Jump")
	void DoubleJump();
	void HorizontalJump();

#pragma endregion 
	

private:
	UPROPERTY(EditAnywhere, Category = "Test")
	TMap<FertilizerType, TSubclassOf<UFertilizerBase>> Effects;
	
	UPROPERTY(EditAnywhere, Category = "Test")
	FertilizerType EFertilizerType;

	TSubclassOf<UFertilizerBase> GetCurrentEffect(FertilizerType Type);

	bool IsCurrentEffectExist(FertilizerType Type);

#pragma endregion 
	
};

