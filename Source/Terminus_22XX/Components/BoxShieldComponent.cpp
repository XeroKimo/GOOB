// Fill out your copyright notice in the Description page of Project Settings.

#include "BoxShieldComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

UBoxShieldComponent::UBoxShieldComponent()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("Hitbox");
	BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	BoxCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);

	ShieldMesh->SetupAttachment(BoxCollider);
}

void UBoxShieldComponent::SetupAttachment(USceneComponent * Scene, FName SocketName)
{
	BoxCollider->SetupAttachment(Scene, SocketName);
}
