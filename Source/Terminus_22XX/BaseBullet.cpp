// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBullet.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABaseBullet::ABaseBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BulletHitbox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	RootComponent = BulletHitbox;
	BulletHitbox->RelativeRotation.Pitch = 90.f;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCollisionProfileName("NoCollision");

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Movement Component");
	InitialLifeSpan = 3.f;
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseBullet::ComponentHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
}

