// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/ShieldGenerator.h"
#include "Guns/BaseBullet.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>("Shield Mesh");
	ShieldMesh->SetCollisionProfileName("NoCollision");
	// ...
	SetIsReplicated(true);
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		for (AShieldGenerator* generator : ArrayOfShieldGenerators)
		{
			generator->AddShield(this);
			if (generator->GetGeneratorIsActive())
				ShieldGeneratorsAlive++;
		}
		OnRep_Generators();
		if (ShieldGeneratorsAlive > 0)
			IsShieldActive = true;
	}

}


// Called every frame
void UShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UShieldComponent::ComponentHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor->ActorHasTag("Bullet"))
	{
		ABaseBullet* bullet = Cast<ABaseBullet>(OtherActor);
		if (CanBeDestroyed && IsShieldActive)
		{
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

void UShieldComponent::DecrementActiveGenerators()
{
	if (ShieldGeneratorsAlive > 0)
	{
		ShieldGeneratorsAlive--;
		OnRep_Generators();
		IsShieldActive = (ShieldGeneratorsAlive > 0) ? true : false;
		if (!IsShieldActive)
			NetMulticastDisableShieldCollisions();
	}
}

void UShieldComponent::ServerDecrementActiveGenerators_Implementation()
{
	DecrementActiveGenerators();
}

bool UShieldComponent::ServerDecrementActiveGenerators_Validate()
{
	return true;
}


void UShieldComponent::NetMulticastDisableShieldCollisions_Implementation()
{
	DisableShieldCollisions();
}

void UShieldComponent::OnRep_Generators()
{
}

void UShieldComponent::EnableShieldCollisions()
{
    ShieldMesh->SetVisibility(true);
}

void UShieldComponent::DisableShieldCollisions()
{
    ShieldMesh->SetVisibility(false);
}

void UShieldComponent::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UShieldComponent, ShieldGeneratorsAlive);
	DOREPLIFETIME(UShieldComponent, IsShieldActive);

}