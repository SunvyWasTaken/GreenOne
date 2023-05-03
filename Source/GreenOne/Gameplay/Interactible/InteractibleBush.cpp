// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleBush.h"

#include "Engine/SkeletalMeshSocket.h"

AInteractibleBush::AInteractibleBush()
{
}

void AInteractibleBush::Action(AActor* Interactor)
{
	Super::Action(Interactor);

	if(USkeletalMeshComponent* ActorSkeletalMesh = Interactor->FindComponentByClass<USkeletalMeshComponent>())
	{
		const USkeletalMeshSocket* Socket = ActorSkeletalMesh->GetSocketByName(FName("socket_hat"));
		if(Socket)
		{
			MeshComponent->SetWorldLocation(Socket->GetSocketLocation(ActorSkeletalMesh));
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AttachToComponent(ActorSkeletalMesh, FAttachmentTransformRules::KeepWorldTransform,FName("socket_hat"));
		}
	}
}
