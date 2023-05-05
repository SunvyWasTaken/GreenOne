// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FertilizerTankComponent.generated.h"

enum class FertilizerType : uint8;
USTRUCT(BlueprintType)
struct FertilizerTankStruct
{
	GENERATED_BODY()

	FertilizerTankStruct();

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties")
	FertilizerType Type;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100), Category = "Custom|Fertilizer|Tank|Properties")
	float MaxGaugeValue = 100.f;
	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|Properties")
	float GaugeValue = 0.f;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100), Category = "Custom|Fertilizer|Tank|Properties")
	float ReduceGaugeValue = 5.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFertilizerBase> Effect;

	UPROPERTY(EditAnywhere, Category = "Custom|Fertilizer|Tank|UI")
	FLinearColor ColorInfo;
	
	void UpdateGauge();
	void AddFertilizer(float NewGaugeValue);
	
private:
	void ClampGaugeValue();
	
};

//TODO: preparer les delegates pour les UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateFertilizerTankGaugeSignature, int, index, float, GaugeValueUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActionFertilizerSignature, int, index, float, GaugeValue, FLinearColor, ColorInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchFertilizerTypeSignature, int, index, float, GaugeValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREENONE_API UFertilizerTankComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFertilizerTankComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
	TMap<FertilizerType,FertilizerTankStruct> FertilizerTanks;

	// FertilizerTankStruct* FertilizerPrimary;
	// FertilizerTankStruct* FertilizerSecondary;
	FertilizerType FertilizerPrimaryType;
	FertilizerType FertilizerSecondaryType;
	
	bool IsTypeExist(const FertilizerType Type) const;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FertilizerType EFertilizerType;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugValues;
	FertilizerTankStruct* Struct;
#endif
	
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnUpdateFertilizerTankGaugeSignature OnUpdateFertilizerTankGaugeDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnActionFertilizerSignature OnActionFertilizerDelegate;

	UPROPERTY(EditAnywhere)
	bool bFertilizerActive = false;
	
	UFUNCTION(BlueprintCallable)
	void OnShoot();

	UFUNCTION(BlueprintCallable)
	bool IsTankEmpty(const FertilizerType Type);

	void UpdateFertilizerType(FertilizerType Type);
	UFUNCTION(BlueprintCallable)
	void InitUIFertilizer();
	void Equip();
	void SwitchFertilizerEquip();

	FertilizerType GetCurrentFertilizerType() const;
	
	UFUNCTION(BlueprintCallable)
	class UFertilizerBase* GetEffect();
	
	FertilizerTankStruct* GetCurrentFertilizerTankActive();
	FertilizerTankStruct* GetFertilizerTankByType(FertilizerType Type);

	/**Only Debug -> To Delete*/
	FString GetFertilizerTypeName() const;

	UFUNCTION(BlueprintCallable)
	void SetFertilizerValueByType(FertilizerType Type, float Value);
	void EventAction();
};
