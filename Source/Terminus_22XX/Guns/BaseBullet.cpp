// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBullet.h"
#include "BaseGun.h"
#include "Networked/NetBaseGun.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Actors/ShieldGenerator.h"

// Sets default values
ABaseBullet::ABaseBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BulletHitbox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	RootComponent = BulletHitbox;
	BulletHitbox->RelativeRotation.Pitch = 90.f;
	BulletHitbox->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	BulletHitbox->OnComponentHit.AddDynamic(this, &ABaseBullet::ComponentHit);
	BulletHitbox->OnComponentBeginOverlap.AddDynamic(this, &ABaseBullet::OnOverlapBegin);
	BulletHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FCollisionResponseContainer responseContainer;
	responseContainer.SetAllChannels(ECollisionResponse::ECR_Ignore);
	responseContainer.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	responseContainer.SetResponse(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	responseContainer.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	responseContainer.SetResponse(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);

	BulletHitbox->SetCollisionResponseToChannels(responseContainer);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCollisionProfileName("NoCollision");

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Movement Component");
	MovementComponent->UpdatedComponent = BulletHitbox;
	MovementComponent->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = BulletLifeSpan;

    SetReplicates(true);
    SetReplicateMovement(true);
	Tags.Add("Bullet");
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseBullet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MovementComponent->InitialSpeed = BulletSpeed;
    MovementComponent->Velocity = GetActorRotation().Vector().GetSafeNormal() * BulletSpeed;
    //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "BulletRotation - " + FString::SanitizeFloat(GetActorRotation().Yaw) + " , " + FString::SanitizeFloat(GetActorRotation().Pitch));
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBullet::ServerSetBulletDamage_Implementation(float NewDamage)
{
    BulletDamage = NewDamage;
}
bool ABaseBullet::ServerSetBulletDamage_Validate(float NewDamage)
{
    return true;
}

void ABaseBullet::NetMulticastSetBulletDirection_Implementation(FVector Direction)
{
	MovementComponent->Velocity = Direction * BulletSpeed;
	BulletDirection = Direction;
}

void ABaseBullet::ComponentHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherActor != GetOwner())
	{
        if (Role == ROLE_Authority)
        {
            FDamageEvent DamageEvent;
            OtherActor->TakeDamage(BulletDamage, DamageEvent, GetInstigatorController(), this);
        }
	}
    if (OtherActor != GetOwner())
	    Destroy();
}

void ABaseBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherActor != GetOwner())
	{
		if (Role == ROLE_Authority)
		{
			FDamageEvent DamageEvent;
			OtherActor->TakeDamage(BulletDamage, DamageEvent, GetInstigatorController(), this);
			Destroy();
		}
	}
}


void ABaseBullet::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABaseBullet, BulletDamage);
}