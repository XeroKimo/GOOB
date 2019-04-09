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
        GeneratorMesh->SetMaterial(0, AliveMaterial);
    }
    else
    {
        GeneratorMesh->SetMaterial(0, DeadMaterial);
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
    if (DamageCauser->GetOwner()->ActorHasTag("Player"))
	{
		if (GeneratorIsActive)
		{
			if (Role == ROLE_Authority)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Damage Received - " + FString::FromInt(Damage));
				GeneratorHealth -= Damage;
				if (GeneratorHealth <= 0)
				{
					GeneratorIsActive = false;
                    NetMulicastChangeMaterial(DeadMaterial);
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
    GeneratorMesh->SetMaterial(0, material);
}


void AShieldGenerator::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AShieldGenerator, GeneratorHealth);
    DOREPLIFETIME(AShieldGenerator, GeneratorIsActive);
    DOREPLIFETIME(AShieldGenerator, AliveMaterial);
    DOREPLIFETIME(AShieldGenerator, DeadMaterial);

}