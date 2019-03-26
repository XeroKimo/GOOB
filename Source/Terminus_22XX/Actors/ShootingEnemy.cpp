// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootingEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Guns/BaseBullet.h"
#include "Engine/World.h"

AShootingEnemy::AShootingEnemy()
{
	PlayerDetection = CreateDefaultSubobject<USphereComponent>("Player Detector");
	PlayerDetection->SetCollisionProfileName("OverlapOnlyPawn");
	PlayerDetection->SetGenerateOverlapEvents(true);
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetSphereRadius(300.0f, true);

	ShootingArrow = CreateDefaultSubobject<UArrowComponent>("Shooting Arrow");
	ShootingArrow->SetupAttachment(RootComponent);

	WasPatrolling = bPatrol;
}
void AShootingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority)
		DetectPlayer();
}

void AShootingEnemy::DetectPlayer()
{
	TArray<AActor*> players;
	PlayerDetection->GetOverlappingActors(players);

	if (players.Num() == 0)
	{
		bPatrol = WasPatrolling;
		return;
	}

	WasPatrolling = bPatrol;
	bPatrol = false;

	FVector PlayerPos = players[0]->GetActorLocation();
	FRotator PlayerRot = players[0]->GetActorRotation();

	FVector PosDifference = PlayerPos - GetActorLocation();
	FMath::VInterpNormalRotationTo(PlayerRot.Vector(), PosDifference.GetUnsafeNormal(), 0.016f, RotationSpeed);
	ShootPlayer();
}

void AShootingEnemy::ShootPlayer()
{
	if (!CanShoot)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ABaseBullet* bullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, ShootingArrow->GetComponentTransform(), SpawnParams);
	if (bullet)
	{
		bullet->BulletDamage = Damage;
	}
	CanShoot = false;
	GetWorldTimerManager().SetTimer(ShootingTimer, this, &AShootingEnemy::ResetShoot, TimeBetweenShots, false);
}

void AShootingEnemy::BeginPlay()
{

}

void AShootingEnemy::ResetShoot()
{
	CanShoot = true;
}
