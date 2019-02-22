// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldGenerator.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShieldComponent.h"


// Sets default values
AShieldGenerator::AShieldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting Up Mesh
	GeneratorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	GeneratorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RootComponent = GeneratorMesh;

	GeneratorHitBox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	GeneratorHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GeneratorHitBox->AttachTo(RootComponent);


	bCanBeDamaged = true;

	OnTakeAnyDamage.AddDynamic(this, &AShieldGenerator::TakeAnyDamage);


}

// Called when the game starts or when spawned
void AShieldGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShieldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShieldGenerator::AddShield(UShieldComponent* Shield)
{
	if (Shield)
		PointersToShields.Add(Shield);
}



void AShieldGenerator::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (DamageCauser->ActorHasTag("ShotgunBullets"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Damage Received - " + FString::FromInt(Damage));
		GeneratorHealth -= Damage;
		if (GeneratorHealth <= 0)
		{
			GeneratorIsActive = false;
		}
		else
		{
			return;
		}
	}
}



