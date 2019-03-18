// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGun.h"

#include "Components/SkeletalMeshComponent.h"
#include "BaseBullet.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Public/TimerManager.h"

#include "Actors/PlayerCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseGun::ABaseGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = SphereCollision;
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseGun::OnOverlapBegin);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Gun Mesh");
	GunMesh->SetupAttachment(SphereCollision);
	GunMesh->SetCollisionProfileName("NoCollision");

	GunMuzzleLocation = CreateDefaultSubobject<UArrowComponent>("Muzzle Location");
	GunMuzzleLocation->SetupAttachment(GunMesh);
	

	CurrentBurstShot = 0;
}

// Called when the game starts or when spawned
void ABaseGun::BeginPlay()
{
	Super::BeginPlay();
	FireRateinSeconds = 60.f / (float)FireRate; 
	TimeSinceLastShot = GetWorld()->TimeSeconds;
	PelletsPerShot = (PelletsPerShot < 1) ? 1 : PelletsPerShot;
	if (GetOwner() != nullptr)
		SphereCollision->SetCollisionProfileName("NoCollision");
	
}

// Called every frame
void ABaseGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ((CurrentClipSize == 0 && CurrentStockAmmo > 0)|| (CurrentClipSize == 0 && MaxStockAmmo < 0))
	{
		StartReloading();
	}

}

void ABaseGun::SpawnBullets()
{
	UWorld* const world = GetOwner()->GetWorld();
	if (world != nullptr)
	{
		for (int i = 0; i < PelletsPerShot; i++)
		{
			FRotator weaponRot = GunMuzzleLocation->GetComponentRotation();
            FVector bulletSpawnRot = FMath::VRandCone(weaponRot.Vector(), FMath::DegreesToRadians(WeaponSpread));
            FRotator temp = bulletSpawnRot.Rotation();

			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "WeaponRotation - " + FString::SanitizeFloat(temp.Yaw) + " , " + FString::SanitizeFloat(temp.Pitch));

			FVector spawnLoc = GunMuzzleLocation->GetComponentLocation();

			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Spawn Location - " + FString::SanitizeFloat(spawnLoc.X) + " , " + FString::SanitizeFloat(spawnLoc.Y) + " , " + FString::SanitizeFloat(spawnLoc.Z));

			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = Cast<APawn>(GetOwner());// OwnedCharacter;

			ABaseBullet* spawnedBullet = world->SpawnActor<ABaseBullet>(BulletClassType, spawnLoc, weaponRot, spawnParams);
			if (spawnedBullet)
			{
				spawnedBullet->ServerSetBulletDamage(BulletDamage);
				spawnedBullet->NetMulticastSetBulletDirection(bulletSpawnRot.GetSafeNormal());
			}
		}
	}
}

void ABaseGun::WillShoot()
{
	CanFire = true;
    //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Can Shoot");
}

void ABaseGun::Fire()
{
	if (BulletClassType != NULL)
	{
        //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Weapon Fired");
		SpawnBullets();
        CanFire = false;
        if (!InfiniteClipSize)
            CurrentClipSize--;
		if (CurrentClipSize == 0)
		{
			StopFiring();
			StartReloading();

			if (ShootingStyle == EFireStyle::FS_Burst)
			{
				GetWorldTimerManager().ClearTimer(FireWeaponTimer);
				CurrentBurstShot = 0;
			}
		}

		if (ShootingStyle == EFireStyle::FS_Burst)
		{
			CurrentBurstShot++;
            //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Current Burst shot = " + FString::FromInt(CurrentBurstShot));
			if (CurrentBurstShot == ShotsPerBurst)
			{
				GetWorldTimerManager().ClearTimer(FireWeaponTimer);
                CurrentBurstShot = 0;
			}
		}

		TimeSinceLastShot = GetWorld()->TimeSeconds;
	}
}

void ABaseGun::StopFiring()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ABaseGun::PullTrigger()
{
	FTimerManager& timerManager = GetWorldTimerManager();
	if (CurrentClipSize > 0 || InfiniteClipSize)
	{
		CheckForWeaponJam();
        if (CanFire)
        {
            if (ShootingStyle == EFireStyle::FS_FullAuto)
            {
                timerManager.SetTimer(FireWeaponTimer, this, &ABaseGun::Fire, FireRateinSeconds, true, 0.f);
            }
            else if (ShootingStyle == EFireStyle::FS_Single)
            {
                Fire();
				if (PumpTime !=0.0f)
					timerManager.SetTimer(CanShootTimer, this, &ABaseGun::WillShoot, PumpTime, false);
				else
					timerManager.SetTimer(CanShootTimer, this, &ABaseGun::WillShoot, FireRateinSeconds, false);
            }
            else if (ShootingStyle == EFireStyle::FS_Burst)
            { 
                timerManager.SetTimer(FireWeaponTimer, this, &ABaseGun::Fire, FireRateinSeconds, true, 0.f);

				float shootingTime = FireRateinSeconds + AdditionalDelayBetweenBursts;
                timerManager.SetTimer(CanShootTimer, this, &ABaseGun::WillShoot, shootingTime, false);
            }
        }
		else
		{
			if (ShootingStyle == EFireStyle::FS_FullAuto)
			{
				float remainingTime = timerManager.GetTimerRemaining(CanShootTimer);
				timerManager.SetTimer(FireWeaponTimer, this, &ABaseGun::Fire, FireRateinSeconds,true, remainingTime);
			}
		}
	}
	else
	{
		StartReloading();
	}
}

void ABaseGun::ReleaseTrigger()
{
	FTimerManager& timerManager = GetWorldTimerManager();
    if (ShootingStyle == EFireStyle::FS_FullAuto && !timerManager.TimerExists(CanShootTimer))
    {
        float remainingTime = timerManager.GetTimerRemaining(FireWeaponTimer);
        timerManager.SetTimer(CanShootTimer, this, &ABaseGun::WillShoot, remainingTime, false);
    }

    if (ShootingStyle!= EFireStyle::FS_Burst)
		timerManager.ClearTimer(FireWeaponTimer);
}

void ABaseGun::Reload()
{
	if (CurrentStockAmmo > 0 && MaxStockAmmo > 0)
	{
		int ammoToTransfer = MaxClipSize - CurrentClipSize;
		CurrentStockAmmo -= ammoToTransfer;

		if (CurrentStockAmmo < 0)
		{
			ammoToTransfer += CurrentStockAmmo;
			CurrentStockAmmo = 0;
		}

		CurrentClipSize += ammoToTransfer;
	}
	else if (MaxStockAmmo < 0)
	{
		int ammoToTransfer = MaxClipSize - CurrentClipSize;
		CurrentClipSize += ammoToTransfer;
	}
	IsReloading = false;
}

void ABaseGun::SetOwnedCharacter(APlayerCharacter * Character)
{
	OwnedCharacter = Character; 
	//GunMesh->SetVisibility(false);
	//SetActorHiddenInGame(true);
}

void ABaseGun::Attach()
{
	AttachToComponent(OwnedCharacter->GetGunScene(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GunMesh->SetVisibility(true);
	SetActorHiddenInGame(false);
}

void ABaseGun::Detach()
{
	GunMesh->SetVisibility(false);
	SetActorHiddenInGame(true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ABaseGun::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
		if (character)
		{
			if (character->AddWeaponToInvetory(this))
				SphereCollision->SetCollisionProfileName("NoCollision");

		}
	}
}

void ABaseGun::CheckForWeaponJam()
{
	float lastShotTime = GetWorld()->TimeSince(TimeSinceLastShot);
	if (lastShotTime > FireRateinSeconds)
		WillShoot();

}

void ABaseGun::StartReloading()
{
	if (!IsReloading && CurrentClipSize < MaxClipSize && !InfiniteClipSize)
	{
		GetWorldTimerManager().SetTimer(ReloadTimer, this, &ABaseGun::Reload, ReloadSpeed, false);
		IsReloading = true;
	}
}