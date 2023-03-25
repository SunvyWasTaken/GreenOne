// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Dodge.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


UBTT_Dodge::UBTT_Dodge()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Dodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(TargetRef.IsNone())
	{
		UE_LOG(LogTemp,Warning, TEXT("PlayerRef not set in the Dodge Task."));
		return EBTNodeResult::Failed;
	}
	ControllerRef = Cast<AController>(OwnerComp.GetOwner());
	if (!ControllerRef)
	{
		UE_LOG(LogTemp,Warning, TEXT("Le cast du controller à fail dans la task Dodge"));
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
	CharaMovementComp->MaxFlySpeed = FlyingSpeedInit*2;
	CharaMovementComp->bOrientRotationToMovement = false;
	if (UKismetMathLibrary::RandomBool())
	{
		DodgeDirection *= -1.f;
	}
	return EBTNodeResult::InProgress;
}

void UBTT_Dodge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	ControllerRef->GetPawn()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ControllerRef->GetPawn()->GetActorLocation(), PlayerRef->GetActorLocation()));
	const double DistanceBetween = FVector::Distance(PlayerRef->GetActorLocation(), ControllerRef->GetPawn()->GetActorLocation());
	FVector DodgeVelocity = ControllerRef->GetPawn()->GetActorRightVector() * DodgeDirection;
	DodgeVelocity.Normalize();
	if (ACharacter* AIRef = Cast<ACharacter>(ControllerRef->GetPawn()))
	{
		
		AIRef->GetMovementComponent()->AddInputVector(DodgeVelocity);
		//UE_LOG(LogTemp,Warning, TEXT("Vector Dodge : %s"), *DodgeVelocity.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task Dodge n'est pas un character est du coup fail l'appel pour le move."));
	}
	const float DotPro = FVector::DotProduct(ControllerRef->GetPawn()->GetActorForwardVector(), PlayerRef->GetActorForwardVector());
	//UE_LOG(LogTemp, Warning, TEXT("Dot : %f"), DotPro);
	if ( DotPro >= DodgeDistance || DotPro >= DodgeDistance*-1)
	{
		CharaMovementComp->MaxFlySpeed = FlyingSpeedInit;
		CharaMovementComp->bOrientRotationToMovement = true;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
