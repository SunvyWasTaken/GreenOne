// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacterMovement.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

void UCustomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GreenOneCharacter = Cast<AGreenOneCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DashTick(DeltaTime);
	CooldownTick(DeltaTime);


#if WITH_EDITOR && (DEBUG_MESSAGE_DASH == AVAILABLE)
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Blue, FString::Printf(TEXT("Dash CoolDown %f"), CurrentDashCooldown), true, FVector2d(1.5, 1.5));
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Red, FString::Printf(TEXT("Dash CoolDownValue %f"), DashCooldown), true, FVector2d(1.5, 1.5));
#endif
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UCustomCharacterMovementComponent::Dash()
{
	// Securite
	if (GreenOneCharacter == nullptr) { return; }
	if (GreenOneCharacter->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	// 

	GreenOneCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_DASH);
	StartDashLocation = GreenOneCharacter->GetActorLocation();

	// TODO : Selon l'input du joueur, on change la direction du dash

	// Récupération de la direction du joueur
	FVector DirectionVector = FVector(0.f, 0.f, 0.f);

	if ( GreenOneCharacter->GetLastMovementInputVector() == DirectionVector)
	{
		DirectionVector = GreenOneCharacter->GetActorForwardVector();
	}
	else
	{
		DirectionVector = GreenOneCharacter->GetLastMovementInputVector();
	}

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	TargetDashLocation = StartDashLocation + DirectionVector * DashDistance;

	UE_LOG(LogTemp, Warning, TEXT("Dash Start : %s"), *StartDashLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Dash Direction : %s"), *DirectionVector.ToString());

	UE_LOG(LogTemp, Warning, TEXT("Dash Location : %s"), *TargetDashLocation.ToString());

	// On verifie si le dash est en collision avec un objet
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDashLocation, TargetDashLocation, ECC_Visibility, CollisionParams);

	// Si le dash est en collision avec un objet, on reduit la distance du dash
	if (bIsHit)
	{
		TargetDashLocation = StartDashLocation + (DirectionVector * (HitResult.Distance - 50.f));
	}

	CurrentDashAlpha = 0.f;
	bIsDashing = true;
}

void UCustomCharacterMovementComponent::DashTick(float deltatime)
{
	// Securite
	if (!bIsDashing || bDashOnCooldown) { return; }
	if (GreenOneCharacter == nullptr) { return; }
	// 

	CurrentDashAlpha += (deltatime * 1000) / (DashTime * 100);

	if (CurrentDashAlpha >= 1)
	{
		CurrentDashAlpha = 1;
		CurrentDashCooldown = DashCooldown;
		bIsDashing = false;
		bDashOnCooldown = true;
		GreenOneCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	FVector TargetLocation = UKismetMathLibrary::VLerp(StartDashLocation, TargetDashLocation, CurrentDashAlpha);
	GreenOneCharacter->SetActorLocation(TargetLocation);

}

void UCustomCharacterMovementComponent::CooldownTick(float deltatime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= deltatime;

	if (CurrentDashCooldown <= 0.f)
	{
		CurrentDashCooldown = 0.f;
		bDashOnCooldown = false;
	}
}