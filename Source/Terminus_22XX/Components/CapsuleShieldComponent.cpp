// Fill out your copyright notice in the Description page of Project Settings.

#include "CapsuleShieldComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

UCapsuleShieldComponent::UCapsuleShieldComponent()
{
	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	CapsuleCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);
	CapsuleCollider->SetupAttachment(this);
	ShieldMesh->SetupAttachment(CapsuleCollider);


	EnabledCollisions = CapsuleCollider->GetCollisionResponseToChannels();
}


void UCapsuleShieldComponent::EnableShieldCollisions()
{
	CapsuleCollider->SetCollisionResponseToChannels(EnabledCollisions);
}
void UCapsuleShieldComponent::DisableShieldCollisions()
{
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
void UCapsuleShieldComponent::BeginPlay()
{
	UShieldComponent::BeginPlay();

	if (!IsShieldActive)
	{
		DisableShieldCollisions();
	}
}