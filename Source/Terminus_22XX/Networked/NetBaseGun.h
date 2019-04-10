// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetBaseGun.generated.h"

UENUM(BlueprintType)
enum class EFireStyle : uint8
{
	FS_Single UMETA(DisplayName = "Single"),
	FS_Burst UMETA(DisplayName = "Burst"),
	FS_FullAuto UMETA(DisplayName = "Auto"),
};

UENUM(BlueprintType)
enum EGunName
{
	GN_Shotgun = 0,
	GN_Vampyr,
	GN_Railgun,
	GN_Max,
};

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

protected:
	//The style of shooting
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		EFireStyle FireStyle = EFireStyle::FS_Single;
	//How much damage the bullet does
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float BulletDamage = 0;
	//The randomness in spread in a cone when spawning bullets
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float BulletSpread = 0;

	//How many bullets have been shot this round of burst
	int BurstCounter;
	//How many bullets will shoot per burst
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int MaxBurstCount = 3;
	//How many pellets will spawn when firing
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int PelletCount = 1;

	//How fast a gun will shoot in round per minute
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		int FireRate = 60;
	//An extra delay after a round of burst shots were complete
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float AdditionalBurstDelay = 0.f;
	//How long it'll take to reload in seconds
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		float ReloadRate = 1.f;

	bool IsTriggerPulled = false;
	bool WasTriggerPulled = false;
	bool IsReloading = false;
	//A boolean so that a gun does not need to reload
	UPROPERTY(Category = "Weapon Config", EditAnywhere)
		bool InfiniteClip = false;
	//Prevents the gun from shooting faster than intended
	bool CanFire = true;

	//The bullet that will spawn
	UPROPERTY(Category = "Ammo Config", EditAnywhere)
		TSubclassOf<class ABaseBullet> BulletClass;

    UPROPERTY(EditAnywhere)
        class USoundBase* ShootingSound;
public:
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int MaxAmmoCount = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated, BlueprintReadOnly)
		int CurrentAmmoCount = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated)
		int MaxAmmoStock = 0;
	UPROPERTY(Category = "Ammo Config", EditAnywhere, Replicated, BlueprintReadOnly)
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
