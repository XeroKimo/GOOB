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
	//Calculate the fire rate in seconds
	FireRateInSeconds = (FireRate == 0) ? 60.f / 0.00001f : 60.f / (float) FireRate;
	//Make sure that at least 1 pellet can be spawned;
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
	//Pull the trigger so the gun shoots
	IsTriggerPulled = true;
}

void ANetBaseGun::ReleaseTrigger()
{
	//Release the trigger so the gun stops shooting
	IsTriggerPulled = false;
}

void ANetBaseGun::FireWeapon()
{
	//Make sure the trigger is pulled before we can actually shoot
	if (!IsTriggerPulled)
		return;
	//We can't shoot while reloading
	if (IsReloading)
		return;
	//We can't shoot if we have no bullets
	if (CurrentAmmoCount == 0 && !InfiniteClip)
		return;
	//Make sure a bullet is in the chamber
	//(Prevents the gun from shooting faster than intended)
	if (!CanFire)
		return;
	//If the gun isn't shooting in full auto, make sure
	//Make sure we can't just hold the trigger down to shoot
	if ((FireStyle == EFireStyle::FS_Single || FireStyle == EFireStyle::FS_Burst) && WasTriggerPulled)
		return;

	if (FireStyle == EFireStyle::FS_Burst)
	{
		//Use a looping timer to call the burst fire method
		//So that we do not need to hold the trigger down to shoot
		//In bursts
		GetWorldTimerManager().SetTimer(ShootingTimer, this, &ANetBaseGun::BurstFire, FireRateInSeconds, true, 0.f);
	}
	else
	{
		//Fire normally
		Fire();
		GetWorldTimerManager().SetTimer(AllowFireTimer, this, &ANetBaseGun::AllowFire, FireRateInSeconds, false);
	}
}

void ANetBaseGun::PrepareReload()
{
	//We can't realod if we have an infinite clip size
	if (InfiniteClip)
		return;
	//Don't reload when we are at max ammo count
	if (CurrentAmmoCount >= MaxAmmoCount)
		return;
	//Can't reload if we have no ammo in stock
	if (CurrentAmmoStock == 0)
		return;
	if (MaxAmmoStock == 0)
		return;

	//Start reloading
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ANetBaseGun::Reload, ReloadRate, false);
	IsReloading = true;
}

void ANetBaseGun::StopFiring()
{
	//Stop shooting
	GetWorldTimerManager().ClearTimer(ShootingTimer);
	//Reset the burst counter
	BurstCounter = 0;
}

void ANetBaseGun::StopReloading()
{
	//Stops reloading
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
	//Allow the gun to shoot again
	CanFire = true;
}

void ANetBaseGun::ServerAttach_Implementation(ANetPlayerCharacter * character)
{
	//Attach weapon to the character
	AttachToComponent(character->GetGunScene(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//Make the weapon visible
	GunMesh->SetVisibility(true);
	SetActorHiddenInGame(false);
}

bool ANetBaseGun::ServerAttach_Validate(ANetPlayerCharacter * character)
{
	return true;
}

void ANetBaseGun::ServerDetach_Implementation()
{
	//Hide the weapon
	GunMesh->SetVisibility(false);
	SetActorHiddenInGame(true);
	//Detach the weapon from the character
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

bool ANetBaseGun::ServerDetach_Validate()
{
	return true;
}

void ANetBaseGun::Fire()
{
	//Set can fire to false to prevent the gun from
	//firing faster than intended
	CanFire = false;
	//Tell the server to shoot
	ServerFire();
}

void ANetBaseGun::BurstFire()
{
	//Call fire normally
	Fire();
	//Increase the burst counter
	BurstCounter++;
	//If we ran out of bullets in the clip, or reached the maximum bullets shot for this burst
	if (BurstCounter > MaxBurstCount - 1 || (CurrentAmmoCount == 0 && !InfiniteClip))
	{
		//Stop shooting
		StopFiring();
		//Allow the gun to fire again after a delay
		if (AdditionalBurstDelay > 0.f)
			GetWorldTimerManager().SetTimer(AllowFireTimer, this, &ANetBaseGun::AllowFire, AdditionalBurstDelay, false);
		else
			AllowFire();
	}
}

void ANetBaseGun::AutoReloadWeapon()
{
	//Reloads weapon when we ran out of bullets in the clip
	if (!CurrentAmmoCount == 0)
		return;

	PrepareReload();
}

void ANetBaseGun::Reload()
{
	//Tell the server to reload the gun
	ServerReload();
	IsReloading = false;
}

void ANetBaseGun::ClientFullStop_Implementation()
{
    FullStop();
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
	//Calculate the amount of bullets required to transfer
	int ammoTransfer = MaxAmmoCount - CurrentAmmoCount;

	if (MaxAmmoStock > 0)
	{
		//Take that amount of bullets away from the stock
		CurrentAmmoStock -= ammoTransfer;
		//If we went to negative
		if (CurrentAmmoStock < 0)
		{
			//Decrease the amount to transfer by the amount
			ammoTransfer += CurrentAmmoStock;
			//Turn current ammo stock to 0
			CurrentAmmoStock = 0;
		}
	}
	//Transfer the ammo to our clip
	CurrentAmmoCount += ammoTransfer;
}

bool ANetBaseGun::ServerReload_Validate()
{
	return true;
}

void ANetBaseGun::ServerFire_Implementation()
{
	//Make sure we have a bullet to spawn
	if (BulletClass != NULL)
	{
		//Make some noise
		NetMulticastFireSound();
		//Spawn the bullets
		ServerSpawnBullets();
		//Reduce the bullets in our clip if we don't have infinite clip
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
	//Spawn a bullet for every pellet
	for (int i = 0; i < PelletCount; i++)
	{
		//Get our muzzle's rotation
		FRotator weaponRot = GunMuzzleLocation->GetComponentRotation();
		//Shoot in a random cone in the direction of said rotation
        FRotator bulletSpawnRot = FMath::VRandCone(weaponRot.Vector(), FMath::DegreesToRadians(BulletSpread / 2.f)).Rotation();
		//Some bug fix, bullets seem to spawn 90 degrees higher than intended
        bulletSpawnRot.Pitch -= 90.f;
		//Get the muzzle's location
		FVector weaponLoc = GunMuzzleLocation->GetComponentLocation();
		
		//Always spawn the bullets
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//Make owner and instigator the person who owns the gun
		spawnParams.Owner = GetOwner();
		spawnParams.Instigator = Cast<APawn>(GetOwner());
        //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "WeaponRotation - " + FString::SanitizeFloat(weaponRot.Yaw) + " , " + FString::SanitizeFloat(weaponRot.Pitch));

        //GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "WeaponRotation - " + FString::SanitizeFloat(bulletSpawnRot.Rotation().Yaw) + " , " + FString::SanitizeFloat(bulletSpawnRot.Rotation().Pitch));

		//Spawn the bullet
		ABaseBullet* spawnedBullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, weaponLoc, bulletSpawnRot, spawnParams);
		if (spawnedBullet)
		{
			//Set the bullet's damage to what the gun has set
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