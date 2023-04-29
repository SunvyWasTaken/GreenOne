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
	FORCEINLINE AGreenOneCharacter* GetOwneChara() const { return GreenOneCharacter; }

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

private:
	FRotator TempRotationCharacter = FRotator::ZeroRotator;
	
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

#pragma region Dash_Hugo

//public:
//
//	// Dash dans la direction de l'input mouvement.
//	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
//	void Dash();
//
//	UFUNCTION(BlueprintCallable, Category = "Custom|Dash")
//	void CancelDash();
//
//	// Distance du dash
//	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Distance du dash", ForceUnits = "cm", ClampMin = 0), Category = "Custom|Dash")
//	float DashDistance = 1000.f; // CM  => 100cm = 1m | 1000cm = 10m
//
//	// Vitesse du Dash
//	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Vitesse du dash", ForceUnits = "cm/s", ClampMin = 0), Category = "Custom|Dash")
//	float DashSpeed = 2000.f; // CM/s
//
	// Temps que va prendre le dash a revenir apres utilisation.
	// Le temps est en secondes.
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Temps de recharge du Dash", ForceUnits = "secs", ClampMin = 0), Category = "Custom|Dash")
	float DashCooldown = 3.0f; // S
//
//
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Dash")
	bool bDashOnCooldown = false;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Dash", meta = (AllowPrivateAccess = "true"))
//	float MaxVerticalAngle = 40.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Dash", meta = (AllowPrivateAccess = "true"))
//	float MaxHorizontalAngle = 60.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Dash", meta = (AllowPrivateAccess = "true"))
//	bool DrawLines = true;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Dash", meta = (AllowPrivateAccess = "true"))
//	float MaxClimbHeight = 30.f;
//
//	/**
//	 * Return the remaining time of the dash cooldown.
//	 */
//	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Keywords = "Cooldown|Dash"), Category = "Dash")
//	float GetRemainingDashTime() { return CurrentDashCooldown; };
//
//
//private:
//
//	// Utiliser pour placer le player pendant le Dash
//	void DashTick(float DeltaTime);
//
	// Cooldown du Dash
	void CooldownTick(float DeltaTime);
//
//	void CollisionTrace(FVector Begin, FVector End);
//
//	bool CanClimbUP(FHitResult &OldHitResult);
//
//	void AngleTrace(FHitResult &HitResult);
//
//	// Vector de Départ
//	FVector StartDashLocation = FVector::ZeroVector;
//
//	// Vector de Fin
//	FVector TargetDashLocation = FVector::ZeroVector;
//
//	// Rotation AVANT
//	FRotator BeforeRotationCharacter = FRotator::ZeroRotator;
//
//	// Direction VECTOR 3D
//	FVector DashDirectionVector3D = FVector::ZeroVector;
//
//
//	// Vector Temporaire pour le Dash
//	FVector TempTargetLocation = FVector::ZeroVector;
//	
//	float CurrentDashAlpha = 0.f;
//
	float CurrentDashCooldown = 0.f;
//
//	float DashTime = 0.f;
//
//	// Distance restante du dash
//	float DashDistanceLeft = 0.f;
//
//	//Classes to ignore when tracing
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
//	TArray<TSubclassOf<AActor>> ClassToSkip;
//
//	//Array of actors to ignore when tracing (added in runtime)
//	UPROPERTY()
//	TArray<AActor*> ToIgnore;
//	
//	//Array of points to dash to
//	TArray<FVector> DashPoints;
//
//	//Capsule component of the player
//	UPROPERTY()
//	UCapsuleComponent* Capsule;
//	
//	//Security to avoid infinite loop
//	int MAXIT = 20;

#pragma endregion
	
#pragma region Keanu

public:

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "IsDashing"), Category = "Custom|Dash")
		bool bIsDashing = false;

	void SetDashDirectionVector(FVector2D& vector) { DashDirectionVector2D = vector; }

	UPROPERTY(EditDefaultsOnly, Category = "Custom|KeanuDash", meta = (ForceUnits = "cm"))
	float CustomDashDistance = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom|KeanuDash", meta = (ForceUnits = "cm/s"))
	float CustomDashSpeed = 2500.f;

	void CustomDash();

private: 

	void CustomDashTick(float Deltatime);
	
	FVector2D DashDirectionVector2D = FVector2D::ZeroVector;

	float PreviousVel;

	FVector CustomForwardVector = FVector::ZeroVector;

	FVector CustomEndLocation = FVector::ZeroVector;

	float CustomTraceParcourtDistance = 0.f;

	float CustomCurrentAlpha = 0;

	TArray<FVector> CustomDashLocation;

};