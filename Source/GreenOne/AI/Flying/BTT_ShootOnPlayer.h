// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShootOnPlayer.generated.h"

UENUM()
enum class EBehavior : uint8
{
	Skip,
	Wait,
	Faild
};

// TODO Add the feature that AI can try to predict where the player will be based on the speed of the bullet.

/**
 *
 */
UCLASS()
class GREENONE_API UBTT_ShootOnPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_ShootOnPlayer();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

private:

	class AFlyingAICharacter* PawnRef;

	bool CheckTargetVisible();

public:

	UPROPERTY(EditAnywhere)
		EBehavior CurrentBehavior;

	/**
	 * Check if there is a collision in front of the AI before shooting and react according to the behavior if it can't shoot.
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Check si il peut tiré"), Category = "CheckCollision")
		bool bCheckCanShoot = false;

	/**
	 * If there is a wall between the AI and the player, the AI's behavior will depend on the programming it has been given.
	 * The Wait behavior will do nothing in particular since it is supposed to wait for something that will never be successful.
	 */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Behavior can shoot", EditCondition = bCheckCanShoot), Category = "CheckCollision")
		EBehavior ShootBehavior;

};
