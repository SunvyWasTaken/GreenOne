// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Dodge.h"
#include "Kismet/KismetMathLibrary.h"
#include "FlyingAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_Dodge::UBTT_Dodge()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_Dodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (TargetRef.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerRef not set in the Dodge Task."));
		return EBTNodeResult::Failed;
	}
	ControllerRef = Cast<AController>(OwnerComp.GetOwner());
	if (!ControllerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Le cast du controller à fail dans la task Dodge"));
		return EBTNodeResult::Failed;
	}
	PlayerRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName));
	if (!PlayerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("le Target ref n'est pas un actor dans le Task Dodge."));
		return EBTNodeResult::Failed;
	}
	CharaMovementComp = Cast<UCharacterMovementComponent>(ControllerRef->GetPawn()->GetMovementComponent());
	if (!CharaMovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast du CharaMovementComp Failed parce que le pawn ne possède pas de character movement"));
		return EBTNodeResult::Failed;
	}
	FlyingSpeedInit = CharaMovementComp->MaxFlySpeed;
	CharaMovementComp->MaxFlySpeed = FlyingSpeedInit * 2;
	CharaMovementComp->bOrientRotationToMovement = false;
	if (UKismetMathLibrary::RandomBool())
	{
		DodgeDirection *= -1.f;
		FVector ori;
		FVector Extend;
		ControllerRef->GetPawn()->GetActorBounds(true, )
	}
	return EBTNodeResult::InProgress;
}

void UBTT_Dodge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ControllerRef->GetPawn()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ControllerRef->GetPawn()->GetActorLocation(), PlayerRef->GetActorLocation()));
	const double DistanceBetween = FVector::Distance(PlayerRef->GetActorLocation(), ControllerRef->GetPawn()->GetActorLocation());
	FVector DodgeVelocity = ControllerRef->GetPawn()->GetActorRightVector() * DodgeDirection + (ControllerRef->GetPawn()->GetActorForwardVector() * -1) * (DistanceBetween <= SafeDistance ? (1) : (0));
	//Normalize the DodgeVelocity vector
	DodgeVelocity.Normalize();

	//Check if the ControllerRef is a character
	if (ACharacter* AIRef = Cast<ACharacter>(ControllerRef->GetPawn()))
	{
		//If it is, get the movement component and add the DodgeVelocity vector to it
		AIRef->GetMovementComponent()->AddInputVector(DodgeVelocity);
		// Check if it's a flying AI to rotate if
		if (AFlyingAICharacter* reffly = Cast<AFlyingAICharacter>(AIRef))
		{
			reffly->SetRotationAxis(FVector2D(DodgeDirection, 0));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task Dodge n'est pas un character est du coup fail l'appel pour le move."));
	}
	// Calculates the dot product of the controller's pawn's forward vector and the player's forward vector.
	const float DotPro = FVector::DotProduct(ControllerRef->GetPawn()->GetActorForwardVector(), PlayerRef->GetActorForwardVector());
	if (bShouldGetSafeDistance)
	{
		if (DotPro >= DodgeDistance || DotPro >= DodgeDistance * -1 && DistanceBetween >= SafeDistance)
		{
			// Set the maximum flying speed of the character movement component to the initial flying speed.
			CharaMovementComp->MaxFlySpeed = FlyingSpeedInit;

			// Set the character movement component to orient the rotation to the movement. 
			CharaMovementComp->bOrientRotationToMovement = true;

			// Finish the latent task with a successful result. 
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		if (DotPro >= DodgeDistance || DotPro >= DodgeDistance * -1 || DistanceBetween >= SafeDistance)
		{
			// Set the maximum flying speed of the character movement component to the initial flying speed.
			CharaMovementComp->MaxFlySpeed = FlyingSpeedInit;

			// Set the character movement component to orient the rotation to the movement. 
			CharaMovementComp->bOrientRotationToMovement = true;

			// Finish the latent task with a successful result. 
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
