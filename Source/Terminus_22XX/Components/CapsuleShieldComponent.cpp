// Fill out your copyright notice in the Description page of Project Settings.

#include "CapsuleShieldComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Guns/BaseBullet.h"

UCapsuleShieldComponent::UCapsuleShieldComponent()
{
	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("Shield Hitbox");
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	CapsuleCollider->OnComponentHit.AddDynamic(this, &UShieldComponent::ComponentHit);
	CapsuleCollider->SetupAttachment(this);

	ShieldMesh->SetupAttachment(CapsuleCollider);

	EnabledCollisions = CapsuleCollider->GetCollisionResponseToChannels();
}

void UCapsuleShieldComponent::EnableShieldCollisions()
{
    UShieldComponent::EnableShieldCollisions();
	CapsuleCollider->SetCollisionResponseToChannels(EnabledCollisions);
}
void UCapsuleShieldComponent::DisableShieldCollisions()
{
    UShieldComponent::DisableShieldCollisions();
	CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
void UCapsuleShieldComponent::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (DamageCauser->ActorHasTag("Bullet"))
	{
		if (CanBeDestroyed && IsShieldActive)
		{
			ABaseBullet* bullet = Cast<ABaseBullet>(DamageCauser);
			CurrentShieldHealth -= bullet->BulletDamage;
			if (CurrentShieldHealth < 0.f)
			{
				CurrentShieldHealth = 0.f;
				IsShieldActive = false;
				DisableShieldCollisions();
			}
		}
	}
}
void UCapsuleShieldComponent::BeginPlay()
{
	UShieldComponent::BeginPlay();

	if (!IsShieldActive)
	{
		DisableShieldCollisions();
	}
}