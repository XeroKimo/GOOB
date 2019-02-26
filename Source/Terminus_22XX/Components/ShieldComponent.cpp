// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/ShieldGenerator.h"
#include "Guns/BaseBullet.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	ShieldMesh->SetCollisionProfileName("NoCollision");
	// ...
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (AShieldGenerator* generator : ArrayOfShieldGenerators)
	{
		generator->AddShield(this);
		if (generator->GetGeneratorIsActive())
			ShieldGeneratorsAlive++;
	}
	if (ShieldGeneratorsAlive > 0)
		IsShieldActive = true;


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
		IsShieldActive = (ShieldGeneratorsAlive > 0) ? true : false;
		if (!IsShieldActive)
			DisableShieldCollisions();
	}
}

void UShieldComponent::EnableShieldCollisions()
{
}

void UShieldComponent::DisableShieldCollisions()
{
}

