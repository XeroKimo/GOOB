// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereShieldComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

USphereShieldComponent::USphereShieldComponent()
{
	SphereCollider = CreateDefaultSubobject<USphereComponent>("Hitbox");
	SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	SphereCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);

	ShieldMesh->SetupAttachment(SphereCollider);
	
}

void USphereShieldComponent::SetupAttachment(USceneComponent * Scene, FName SocketName)
{
	SphereCollider->SetupAttachment(Scene, SocketName);
}
