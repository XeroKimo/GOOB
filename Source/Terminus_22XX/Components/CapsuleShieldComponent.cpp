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

	ShieldMesh->SetupAttachment(CapsuleCollider);
}

void UCapsuleShieldComponent::SetupAttachment(USceneComponent * Scene, FName SocketName)
{
	CapsuleCollider->SetupAttachment(Scene, SocketName);
}
