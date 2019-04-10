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

	//The original state of patrolling
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
	//Find all players overlapping with the player
	TArray<AActor*> players;
	PlayerDetection->GetOverlappingActors(players,ANetPlayerCharacter::StaticClass());

	//If there are no players
	//Go back to the original patrol state
	if (players.Num() == 0)
	{
		bPatrol = WasPatrolling;
		return;
	}

	//Set the patrol state to false and store it's previous state
	WasPatrolling = bPatrol;
	bPatrol = false;

	FVector PlayerPos = players[0]->GetActorLocation();

	FVector PosDifference = PlayerPos - GetActorLocation();
	//SetActorRotation(PosDifference.Rotation());
	//Rotate towards the player
    SetActorRotation(FMath::VInterpNormalRotationTo(GetActorRotation().Vector(), PosDifference.GetUnsafeNormal(), 0.016f, RotationSpeed).Rotation());
	ShootPlayer();
}

void AShootingEnemy::ShootPlayer()
{
	//If the enemy can't shoot, don't shoot
	if (!CanShoot)
		return;

	//Get our gun's spawn location and rotation
    FVector spawnLoc = ShootingArrow->GetComponentLocation();
    FRotator spawnRot = ShootingArrow->GetComponentRotation();
	//Bug fix, bullets spawn 90 degrees more than intended
    spawnRot.Pitch -= 90.0f;

	//Always spawn the bullet and set the owner and instigator to this
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	//Tell world to spawn the bullet
	ABaseBullet* bullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, spawnLoc, spawnRot, SpawnParams);
	if (bullet)
	{
		//Set bullet damage to damage
		bullet->BulletDamage = Damage;
	}
	//Make sure we don't shoot again by setting can shoot to false
	CanShoot = false;
	//Start a timer so we can shoot again
	GetWorldTimerManager().SetTimer(ShootingTimer, this, &AShootingEnemy::ResetShoot, TimeBetweenShots, false);
}

void AShootingEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AShootingEnemy::ResetShoot()
{
	//Allow the enemy to shoot again
	CanShoot = true;
}
