// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/ShieldGenerator.h"


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
	for (AShieldGenerator* generator: ArrayOfShieldGenerators)
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

void UShieldComponent::OnTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (CanBeDestroyed && IsShieldActive)
	{
		CurrentShieldHealth -= Damage;
		if (CurrentShieldHealth < 0.f)
		{
			CurrentShieldHealth = 0.f;
			IsShieldActive = false;
		}
	}
}

void UShieldComponent::SetupAttachment(USceneComponent * scene)
{
}

void UShieldComponent::DecrementActiveGenerators()
{
	if (ShieldGeneratorsAlive > 0)
	{
		ShieldGeneratorsAlive--;
		IsShieldActive = (ShieldGeneratorsAlive > 0) ? true : false;
	}
}

