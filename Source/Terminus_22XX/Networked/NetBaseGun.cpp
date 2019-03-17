// Fill out your copyright notice in the Description page of Project Settings.

#include "NetBaseGun.h"
#include "Public/TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Guns/BaseBullet.h"
#include "NetPlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
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

	//if (Role == ROLE_Authority)
	//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Fire Weapon Authority");
	//if (Role == ROLE_AutonomousProxy)
	//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Fire Weapon Autonomous");
	//if (Role == ROLE_SimulatedProxy)
	//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Fire Weapon Simulated");

	//ACharacter* test = Cast<ACharacter>(GetOwner());
	//if (test)
	//{
	//	if (test->GetController()->IsLocalController())
	//		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Fire Weapon Local");
	//	else
	//		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Fire Weapon Non-Local");

	//}

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
		FVector bulletSpawnRot = FMath::VRandCone(weaponRot.Vector(), FMath::DegreesToRadians(BulletSpread / 2.f));
		FVector weaponLoc = GunMuzzleLocation->GetComponentLocation();
		
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Gun Rotation - " + FString::SanitizeFloat(weaponRot.Yaw) + " , " + FString::SanitizeFloat(weaponRot.Pitch));
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Spawn Location - " + FString::SanitizeFloat(weaponLoc.X) + " , " + FString::SanitizeFloat(weaponLoc.Y) + " , " + FString::SanitizeFloat(weaponLoc.Z));

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParams.Owner = GetOwner();
		spawnParams.Instigator = Cast<APawn>(GetOwner());

		ABaseBullet* spawnedBullet = GetWorld()->SpawnActor<ABaseBullet>(BulletClass, weaponLoc, weaponRot, spawnParams);
		if (spawnedBullet)
		{
			spawnedBullet->SetBulletDamage(BulletDamage);
			spawnedBullet->SetBulletDirection(bulletSpawnRot.GetSafeNormal());
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