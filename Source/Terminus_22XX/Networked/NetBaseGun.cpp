// Fill out your copyright notice in the Description page of Project Settings.

#include "NetBaseGun.h"
#include "Public/TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Guns/BaseBullet.h"
#include "NetPlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

// Sets default values
ANetBaseGun::ANetBaseGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>("Gun Mesh");
	GunMesh->SetupAttachment(RootComponent);
	GunMesh->SetCollisionProfileName("NoCollision");

	GunMuzzleLocation = CreateDefaultSubobject<UArrowComponent>("Muzzle Location");
	GunMuzzleLocation->SetupAttachment(GunMesh);

    AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
    AudioComponent->SetupAttachment(RootComponent);


	BurstCounter = 0;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ANetBaseGun::BeginPlay()
{
	Super::BeginPlay();
	FireRateInSeconds = (FireRate == 0) ? 60.f / 0.00001f : 60.f / (float) FireRate;
	PelletCount = (PelletCount < 1) ? 1 : PelletCount;
	
}

// Called every frame
void ANetBaseGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FireWeapon();
	AutoReloadWeapon();
	WasTriggerPulled = IsTriggerPulled;
}

void ANetBaseGun::PullTrigger()
{
	IsTriggerPulled = true;
}

void ANetBaseGun::ReleaseTrigger()
{
	IsTriggerPulled = false;
}

void ANetBaseGun::FireWeapon()
{
	if (!IsTriggerPulled)
		return;
	if (IsReloading)
		return;
	if (CurrentAmmoCount == 0 && !InfiniteClip)
		return;
	if (!CanFire)
		return;
	if ((FireStyle == EFireStyle::FS_Single || FireStyle == EFireStyle::FS_Burst) && WasTriggerPulled)
		return;

	if (FireStyle == EFireStyle::FS_Burst)
	{
		GetWorldTimerManager().SetTimer(ShootingTimer, this, &ANetBaseGun::BurstFire, FireRateInSeconds, true, 0.f);
	}
	else
	{
		Fire();
		GetWorldTimerManager().SetTimer(AllowFireTimer, this, &ANetBaseGun::AllowFire, FireRateInSeconds, false);
	}
}

void ANetBaseGun::PrepareReload()
{
	if (InfiniteClip)
		return;
	if (CurrentAmmoCount >= MaxAmmoCount)
		return;
	if (CurrentAmmoStock == 0)
		return;
	if (MaxAmmoStock == 0)
		return;

	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ANetBaseGun::Reload, ReloadRate, false);
	IsReloading = true;
}

void ANetBaseGun::StopFiring()
{
	GetWorldTimerManager().ClearTimer(ShootingTimer);
	BurstCounter = 0;
}

void ANetBaseGun::StopReloading()
{
	GetWorldTimerManager().ClearTimer(ReloadTimer);
	IsReloading = false;
}

void ANetBaseGun::FullStop()
{
	ReleaseTrigger();
	StopFiring();
	StopReloading();
}

void ANetBaseGun::AllowFire()
{
	CanFire = true;
}

void ANetBaseGun::ServerAttach_Implementation(ANetPlayerCharacter * character)
{
	AttachToComponent(character->GetGunScene(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GunMesh->SetVisibility(true);
	SetActorHiddenInGame(false);
}

bool ANetBaseGun::ServerAttach_Validate(ANetPlayerCharacter * character)
{
	return true;
}

void ANetBaseGun::ServerDetach_Implementation()
{
	GunMesh->SetVisibility(false);
	SetActorHiddenInGame(true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

bool ANetBaseGun::ServerDetach_Validate()
{
	return true;
}

void ANetBaseGun::Fire()
{
	CanFire = false;
	ServerFire();
}

void ANetBaseGun::BurstFire()
{
	Fire();
	BurstCounter++;
	if (BurstCounter > MaxBurstCount - 1 || (CurrentAmmoCount == 0 && !InfiniteClip))
	{
		StopFiring();
		if (AdditionalBurstDelay > 0.f)
			GetWorldTimerManager().SetTimer(AllowFireTimer, this, &ANetBaseGun::AllowFire, AdditionalBurstDelay, false);
		else
			AllowFire();
	}
}

void ANetBaseGun::AutoReloadWeapon()
{
	if (!CurrentAmmoCount == 0)
		return;

	PrepareReload();
}

void ANetBaseGun::Reload()
{
	ServerReload();
	IsReloading = false;
}

void ANetBaseGun::NetMulticastFireSound_Implementation()
{
	if (ShootingSound)
	{
		AudioComponent->SetSound(ShootingSound);
		AudioComponent->Play();
	}
}


void ANetBaseGun::ServerReload_Implementation()
{
	int ammoTransfer = MaxAmmoCount - CurrentAmmoCount;

	if (MaxAmmoStock > 0)
	{
		CurrentAmmoStock -= ammoTransfer;
		if (CurrentAmmoStock < 0)
		{
			ammoTransfer += CurrentAmmoStock;
			CurrentAmmoStock = 0;
		}
	}

	CurrentAmmoCount += ammoTransfer;
}

bool ANetBaseGun::ServerReload_Validate()
{
	return true;
}

void ANetBaseGun::ServerFire_Implementation()
{
	if (BulletClass != NULL)
	{
		NetMulticastFireSound();
		ServerSpawnBullets();
		if (!InfiniteClip)
			CurrentAmmoCount--;
	}
}

bool ANetBaseGun::ServerFire_Validate()
{
	return true;
}

void ANetBaseGun::ServerSpawnBullets_Implementation()
{
	for (int i = 0; i < PelletCount; i++)
	{
		FRotator weaponRot = GunMuzzleLocation->GetComponentRotation();
        FRotator bulletSpawnRot = FMath::VRandCone(weaponRot.Vector(), FMath::DegreesToRadians(BulletSpread / 2.f)).Rotation();
        bulletSpawnRot.Pitch -= 90.f;
		FVector weaponLoc = GunMuzzleLocation->GetComponentLocation();
		
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		spawnParams.Owner = GetOwner();
		spawnParams.Instigator = Cast<APawn>(GetOwner());
        //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "WeaponRotation - " + FString::SanitizeFloat(weaponRot.Yaw) + " , " + FString::SanitizeFloat(weaponRot.Pitch));

        //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "WeaponRotation - " + FString::SanitizeFloat(bulletSpawnRot.Rotation().Yaw) + " , " + FString::SanitizeFloat(bulletSpawnRot.Rotation().Pitch));

		ABaseBullet* spawnedBullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, weaponLoc, bulletSpawnRot, spawnParams);
		if (spawnedBullet)
		{
			spawnedBullet->ServerSetBulletDamage(BulletDamage);
			//spawnedBullet->NetMulticastSetBulletDirection(bulletSpawnRot.Vector());
		}

	}
}

bool ANetBaseGun::ServerSpawnBullets_Validate()
{
	return true;
}


void ANetBaseGun::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetBaseGun, MaxAmmoCount);
	DOREPLIFETIME(ANetBaseGun, CurrentAmmoCount);
	DOREPLIFETIME(ANetBaseGun, MaxAmmoStock);
	DOREPLIFETIME(ANetBaseGun, CurrentAmmoStock);
	DOREPLIFETIME(ANetBaseGun, FireRateInSeconds);
}