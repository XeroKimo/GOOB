// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldGenerator.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShieldComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AShieldGenerator::AShieldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GeneratorHitBox = CreateDefaultSubobject<UBoxComponent>("Hitbox");
	GeneratorHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeneratorHitBox->SetCollisionProfileName("BlockAll");
	RootComponent = GeneratorHitBox;

	//Setting Up Mesh
	GeneratorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	GeneratorMesh->SetCollisionProfileName("NoCollision");
	GeneratorMesh->SetupAttachment(RootComponent);

    GeneratorMesh->SetIsReplicated(true);

	bCanBeDamaged = true;

	OnTakeAnyDamage.AddDynamic(this, &AShieldGenerator::TakeAnyDamage);

    SetReplicates(true);
}

// Called when the game starts or when spawned
void AShieldGenerator::BeginPlay()
{
	Super::BeginPlay();
    if (GeneratorIsActive)
    {
		NetMulicastChangeMaterial(AliveMaterial);
    }
    else
    {
		NetMulicastChangeMaterial(DeadMaterial);
    }
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
	//if (DamageCauser->ActorHasTag("ShotgunBullet"))
	//If the damage was done by a player
    if (DamageCauser->GetOwner()->ActorHasTag("Player"))
	{
		if (GeneratorIsActive)
		{
			if (Role == ROLE_Authority)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Damage Received - " + FString::FromInt(Damage));
				//Take damage
				GeneratorHealth -= Damage;
				//If we have no health left
				if (GeneratorHealth <= 0)
				{
					//Generator is no longer active
					GeneratorIsActive = false;
					//Change the material to our dead one
                    NetMulicastChangeMaterial(DeadMaterial);
					//Tell all shield components that are pointed to this
					//To reduce the amount of active generators
					for (UShieldComponent* shield : PointersToShields)
						shield->ServerDecrementActiveGenerators();
				}
				else
				{
					return;
				}
			}
		}
	}
}

void AShieldGenerator::NetMulicastChangeMaterial_Implementation(UMaterialInterface * material)
{
    GeneratorMesh->SetMaterialByName("Core", material);
}


void AShieldGenerator::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AShieldGenerator, GeneratorHealth);
    DOREPLIFETIME(AShieldGenerator, GeneratorIsActive);
    DOREPLIFETIME(AShieldGenerator, AliveMaterial);
    DOREPLIFETIME(AShieldGenerator, DeadMaterial);

}