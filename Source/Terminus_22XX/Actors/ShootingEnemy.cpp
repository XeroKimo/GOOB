// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootingEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Guns/BaseBullet.h"
#include "Engine/World.h"
#include "Networked/NetPlayerCharacter.h"

AShootingEnemy::AShootingEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
	PlayerDetection = CreateDefaultSubobject<USphereComponent>("Player Detector");
	PlayerDetection->SetCollisionProfileName("OverlapOnlyPawn");
	PlayerDetection->SetGenerateOverlapEvents(true);
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetSphereRadius(300.0f, true);

    FCollisionResponseContainer responseContainer;
    responseContainer.SetAllChannels(ECollisionResponse::ECR_Ignore);
    responseContainer.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    PlayerDetection->SetCollisionResponseToChannels(responseContainer);


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
	PlayerDetection->GetOverlappingActors(players,ANetPlayerCharacter::StaticClass());

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
	//SetActorRotation(PosDifference.Rotation());
    SetActorRotation(FMath::VInterpNormalRotationTo(GetActorRotation().Vector(), PosDifference.GetUnsafeNormal(), 0.016f, RotationSpeed).Rotation());
	ShootPlayer();
}

void AShootingEnemy::ShootPlayer()
{
	if (!CanShoot)
		return;

    FVector spawnLoc = ShootingArrow->GetComponentLocation();
    FRotator spawnRot = ShootingArrow->GetComponentRotation();
    spawnRot.Pitch -= 90.0f;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ABaseBullet* bullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, spawnLoc, spawnRot, SpawnParams);
	if (bullet)
	{
		bullet->BulletDamage = Damage;
	}
	CanShoot = false;
	GetWorldTimerManager().SetTimer(ShootingTimer, this, &AShootingEnemy::ResetShoot, TimeBetweenShots, false);
}

void AShootingEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AShootingEnemy::ResetShoot()
{
	CanShoot = true;
}
