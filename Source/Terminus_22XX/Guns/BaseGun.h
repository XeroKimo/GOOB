// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickUpActor.h"
#include "BaseGun.generated.h"

// ---------- DEPRECATED CLASS ---------- //
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
class TERMINUS_22XX_API ABaseGun : public APickUpActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnBullets();
	void WillShoot();
	void Fire();
	void StopFiring();

	void PullTrigger();
	void ReleaseTrigger();

	void Reload();
	void StartReloading();


protected:

	UPROPERTY(Category = "Visual", VisibleAnywhere)
		class USkeletalMeshComponent* GunMesh;

	UPROPERTY(Category = "Gun", VisibleAnywhere)
		class UArrowComponent* GunMuzzleLocation;

	UPROPERTY(Category = "Gun", EditAnywhere)
		TEnumAsByte<EFireStyle> ShootingStyle;

	UPROPERTY(Category = "Gun", BlueprintReadOnly)
		int WeaponIndex = -1;
	//How many bullets will be shot in a minute
	UPROPERTY(Category = "Gun", EditAnywhere)
		int FireRate;
	//Randomizes the angle at which the bullets spawn
	UPROPERTY(Category = "Gun", EditAnywhere)
		float WeaponSpread;

	UPROPERTY(Category = "Gun", EditAnywhere)
		float ReloadSpeed;
	//If using burst mode, how many bullets will shoot
	UPROPERTY(Category = "Gun", EditAnywhere)
		int ShotsPerBurst;	
	//How many pellets will shoot per bullet
	UPROPERTY(Category = "Gun", EditAnywhere)
		int PelletsPerShot;

    UPROPERTY(Category = "Gun", EditAnywhere)
        float AdditionalDelayBetweenBursts;
	//How long it'll take before a gun can shoot, will be used over fire rate if > 0 value on single fire style
	UPROPERTY(Category = "Gun", EditAnywhere)
		float PumpTime;

	UPROPERTY(Category = "Ammo", EditAnywhere)
		TSubclassOf<class ABaseBullet> BulletClassType;

	UPROPERTY(Category = "Ammo", EditAnywhere)
		bool InfiniteClipSize = false;
	
	UPROPERTY(Category = "Ammo", EditAnywhere)
		float BulletDamage;
	//Negative value = Infinite Stock Ammo
	UPROPERTY(Category = "Ammo", EditAnywhere)
		int MaxStockAmmo;
	//How spare bullets left to reload into a clip
	UPROPERTY(Category = "Ammo", EditAnywhere)
		int CurrentStockAmmo;
	//How many bullets can be shot before reloading
	UPROPERTY(Category = "Ammo", EditAnywhere)
		int MaxClipSize;
	//How many bullets left in a clip
	UPROPERTY(Category = "Ammo", EditAnywhere)
		int CurrentClipSize;

public:
	bool CanFire = true;
	bool IsReloading = false;


protected:
	FTimerHandle FireWeaponTimer;
	FTimerHandle CanShootTimer;
	FTimerHandle ReloadTimer;

	int CurrentBurstShot = 0;
	float TimeSinceLastShot;
	float FireRateinSeconds;

	class APlayerCharacter* OwnedCharacter;
	
public:
		void SetOwnedCharacter(class APlayerCharacter* Character);
		void Attach();
		void Detach();
		int GetWeaponIndex() { return WeaponIndex; }
		
		UFUNCTION()
			void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
				AActor* OtherActor,
				UPrimitiveComponent* OtherComp,
				int32 OtherBodyIndex,
				bool bFromSweep,
				const FHitResult &SweepResult);

private:
	void CheckForWeaponJam();

};
