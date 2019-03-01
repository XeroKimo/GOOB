// Fill out your copyright notice in the Description page of Project Settings.

#include "BoxShieldComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

UBoxShieldComponent::UBoxShieldComponent()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("Shield Hitbox");
	BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BoxCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);
	BoxCollider->SetupAttachment(this);

	ShieldMesh->SetupAttachment(BoxCollider);

	EnabledCollisions = BoxCollider->GetCollisionResponseToChannels();
}
void UBoxShieldComponent::EnableShieldCollisions()
{
    UShieldComponent::EnableShieldCollisions();
	BoxCollider->SetCollisionResponseToChannels(EnabledCollisions);
}
void UBoxShieldComponent::DisableShieldCollisions()
{
    UShieldComponent::DisableShieldCollisions();
	BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
void UBoxShieldComponent::BeginPlay()
{
	UShieldComponent::BeginPlay();

	if (!IsShieldActive)
	{
		DisableShieldCollisions();
	}
}