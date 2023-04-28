// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEFINE.h"
#include "MACRO.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_NONE	UMETA(Hidden),
	//TODO: Add Custom movement mode
    CMOVE_DASH UMETA(DisplayName = "Dash"),
	CMOVE_MAX	UMETA(Hidden),
};

UENUM()
enum EJumpState
{
	JS_None,
	JS_Vertical,
	JS_Horizontal,
};

/**
 * 
 */
UCLASS()
class GREENONE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	class AGreenOneCharacter* GreenOneCharacter;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGreenOneCharacter* GetOwnerCharacter() const { return GreenOneCharacter; }

protected:
	virtual void InitializeComponent() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	
public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
#pragma region Jump/Falling
	//Attributes//
private:
	int32 MaxJump = 2;

	EJumpState InJumpState = JS_None;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Editer la rapidité du jump vertical")
	bool bManualVerticalVelocity;
	/** Default value of vertical jump is the same that jump velocity */
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Force d'impulsion du jump vertical", meta = (ForceUnits = "cm/s",  EditCondition="bManualVerticalVelocity"))
	float VerticalJumpVelocity = 1000.f;
	float VelocityTemp;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Hauteur max", meta = (ForceUnits = "cm"))
	float MaxVerticalHeight = 800.f;
	bool bVerticalJump;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float SafeZone = 1.f;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Type de curve de jump")
	TEnumAsByte<EEasingFunc::Type> VerticalJumpCurve = EEasingFunc::CircularOut;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Activer la détection du plafond")
	bool bActiveCheckRoof = false;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", DisplayName = "Editer la rapidité du jump horizontal")
	bool bManualHorizontalVelocity = false;
	/** Default value of horizontal jump is the same that jump velocity */
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", meta = (ForceUnits = "cm/s", EditCondition="bManualHorizontalVelocity"), DisplayName = "Rapidité du jump horizontal")
	float HorizontalJumpVelocity = 700.f;
	bool bHorizontalJump;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", meta = (ForceUnits = "cm/s"), DisplayName = "Distance du jump horizontal")
	float MaxDistanceHorizontalJump = 350.f;
	float DistanceHorizontalJump;

	FVector2D HorizontalJumpDirection = FVector2D::ZeroVector;
	float TargetDistance = 0;

	FVector CurrentLocation;
	FVector TargetJumpLocation;

	float JumpTime = 0;
	
	FTimerHandle BlockCheckHandle;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|BlockingCheck")
	float DelayToBlockCheck = .3f;

public:
	/** Commun a tous les jumps **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Jump/Falling", DisplayName = "Velocité général", meta = (ForceUnits = "cm/s"))
	float JumpVelocity = 700.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling", DisplayName = "Jump Gravité")
	float CustomGravityScale = 1.75f;

	//Functions//
private:
	virtual bool CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit, const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations, bool bMustJump) override;
	bool VerticalJump();
	bool HorizontalJump();
	void ExecVerticalJump(const float DeltaTime);
	void ExecHorizontalJump();
	
	FRotator GetRotationToDirection(FVector Direction);

public:
	UFUNCTION(BlueprintCallable)
	virtual bool DoJump(bool bReplayingMoves) override;
	UFUNCTION(BlueprintCallable)
	bool DoHorizontalJump() const;
	UFUNCTION(BlueprintCallable)
	EJumpState GetCurrentJumpState() const;
	UFUNCTION(BlueprintCallable)
	void SetHorizontalJumpDirection(FVector2D& NewDirection);

#pragma endregion 

#pragma region Dash
public:

	// Dash dans la direction de l'input mouvement.
	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
		void Dash();

	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
		void CancelDash();

	// Distance du dash
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Distance du dash", ForceUnits = "cm", ClampMin = 0), Category = "Custom|Dash")
		float DashDistance = 1000.f; // CM  => 100cm = 1m | 1000cm = 10m

	// Vitesse du Dash
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Vitesse du dash", ForceUnits = "cm/s", ClampMin = 0), Category = "Custom|Dash")
		float DashSpeed = 2000.f; // CM/s

	// Temps que va prendre le dash a revenir apres utilisation.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps de recharge du Dash", ForceUnits = "secs", ClampMin = 0), Category = "Custom|Dash")
		float DashCooldown = 3.0f; // S

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "IsDashing"), Category = "Custom|Dash")
		bool bIsDashing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Custom|Dash")
		bool bDashOnCooldown = false;

	/**
	 * Return the remaining time of the dash cooldown.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Keywords = "Cooldown|Dash"), Category = "Dash")
		float GetRemainingDashTime() { return CurrentDashCooldown; };

	void SetDashDirectionVector(FVector2D& vector) { this->DashDirectionVector = vector; }

private:

	// Utiliser pour placer le player pendant le Dash
	void DashTick(float DeltaTime);

	// Cooldown du Dash
	void CooldownTick(float DeltaTime);

	FVector TargetDashLocation = FVector::ZeroVector;

	FVector StartDashLocation = FVector::ZeroVector;

	FRotator BeforeRotationCharacter = FRotator::ZeroRotator;

	FRotator TempRotationCharacter = FRotator::ZeroRotator;

	FVector2D DashDirectionVector = FVector2D::ZeroVector;

	FVector TempTargetLocation = FVector::ZeroVector;

	float CurrentDashAlpha = 0.f;

	float CurrentDashCooldown = 0.f;

	float DashTime = 0.f;

	class USoundBase* DashSound;

#pragma endregion Dash

};