// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Guns/BaseGun.h"
#include "NetBaseGun.generated.h"

//UENUM(BlueprintType)
//enum class EFireStyle : uint8
//{
//	FS_Single UMETA(DisplayName = "Single"),
//	FS_Burst UMETA(DisplayName = "Burst"),
//	FS_FullAuto UMETA(DisplayName = "Auto"),
//};
//
//UENUM(BlueprintType)
//enum EGunName
//{
//	GN_Shotgun = 0,
//	GN_Vampyr,
//	GN_Railgun,
//	GN_Max,
//};

UCLASS()
class TERMINUS_22XX_API ANetBaseGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetBaseGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		EFireStyle FireStyle = EFireStyle::FS_Single;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float BulletDamage = 0;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float BulletSpread = 0;

	int BurstCounter;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int MaxBurstCount = 3;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int PelletCount = 1;

	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int FireRate = 60;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float AdditionalBurstDelay = 0.f;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float ReloadRate = 1.f;

	bool IsTriggerPulled = false;
	bool WasTriggerPulled = false;
	bool IsReloading = false;
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		bool InfiniteClip = false;
	bool CanFire = true;

	UPROPERTY(Category = "Ammo Config", EditAnywhere)
		TSubclassOf<ABaseBullet> BulletClass;

    UPROPERTY(EditAnywhere)
        class USoundBase* ShootingSound;
public:
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int MaxAmmoCount = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int CurrentAmmoCount = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int MaxAmmoStock = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int CurrentAmmoStock = 0;


protected:
	UPROPERTY(Category = "Visual", VisibleAnywhere)
		class UStaticMeshComponent* GunMesh;
	UPROPERTY(VisibleAnywhere)
		class UArrowComponent* GunMuzzleLocation;
    UPROPERTY(VisibleAnywhere)
        class UAudioComponent* AudioComponent;

    int WeaponIndex = -1;
	FTimerHandle ShootingTimer;
	FTimerHandle AllowFireTimer;
	FTimerHandle ReloadTimer;

protected:
	UPROPERTY(Replicated)
		float FireRateInSeconds;

public:
	void PullTrigger();
	void ReleaseTrigger();
	void FireWeapon();

	void AllowFire();
	void PrepareReload();

	void StopFiring();
	void StopReloading();
	void FullStop();

    int GetWeaponIndex() { return WeaponIndex; }
public:
    UFUNCTION(Client, Reliable)
        void ClientFullStop();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAttach(class ANetPlayerCharacter* character);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDetach();
private:
	void Fire();
	void BurstFire();
	void AutoReloadWeapon();
	void Reload();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnBullets();
	UFUNCTION(NetMulticast, Unreliable)
		void NetMulticastFireSound();
};
