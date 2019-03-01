// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereShieldComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

USphereShieldComponent::USphereShieldComponent()
{
	SphereCollider = CreateDefaultSubobject<USphereComponent>("Shield Hitbox");
	SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	SphereCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);
	SphereCollider->SetupAttachment(this);
	ShieldMesh->SetupAttachment(SphereCollider);
	
	EnabledCollisions = SphereCollider->GetCollisionResponseToChannels();
}

void USphereShieldComponent::EnableShieldCollisions()
{
    UShieldComponent::EnableShieldCollisions();
	SphereCollider->SetCollisionResponseToChannels(EnabledCollisions);
}
void USphereShieldComponent::DisableShieldCollisions()
{
    UShieldComponent::DisableShieldCollisions();
	SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
void USphereShieldComponent::BeginPlay()
{
	UShieldComponent::BeginPlay();

	if (!IsShieldActive)
	{
		DisableShieldCollisions();
	}
}