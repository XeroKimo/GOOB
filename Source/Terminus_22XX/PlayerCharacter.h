// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class TERMINUS_22XX_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void MoveForeward(float Val);
	void MoveSideways(float Val);
	void LookUp(float Val);
	void LookSideways(float Val);

	void Jump() override;
	void StopJumping() override;
	void FireWeapon();
	void StopFireWeapon();
	void Reload();

	void SwitchWeapon(float Val);
	void NextWeapon();
	void PreviousWeapon();
	void SwitchToShotgun();
	void SwitchToVampyr();
	void SwitchToRailgun();
	
	void ForceStopAndSlowDescent();

	class USkeletalMeshComponent* GetCharacterMesh() { return CharacterMesh; }
	class USceneComponent* GetGunScene() { return GunLocation; }
private:
	void StoreCurrentSpeed();

public:
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxWalkSpeed = 1000.0f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseTurnSpeed = 1.0f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxAcceleration = 2048.f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float JumpZVelocity = 1000.f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseSuperJumpMultiplier = 0.3f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float AddedSuperJumpMultiplier = 1.2f;

	//The normal Gravity scale
	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseGravityScale = 2.0f;

	//The Gravity Scale when player prepares a super jump
	UPROPERTY(Category = "Movement", EditAnywhere)
		float WeakenedGravityScale = 0.05f;

	//This is a multiplier, taking the power being JumpVelocity * JumpForce * MaxSuperJumpPowerScale
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxSuperJumpPowerScale = 2.0f;

	UPROPERTY(Category = "Status", EditAnywhere)
		float MaxHealth;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		class ABaseGun* CurrentWeapon;

	//How much time before the super jump auto releases
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float ReleaseSuperJumpTime = 1.0f;

	//How much time before the super jump starts preparing
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float SuperJumpTimerDelay = 0.15f;

	//How much time it takes for super jump to reach max power
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float MaxPowerSuperJumpTime = 0.5f;

	float CurrentHealth;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		TArray<TSubclassOf<ABaseGun>> DebugWeapons;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		bool EnableWeaponScrolling = false;
private:
	UPROPERTY(Category = "Visual", VisibleAnywhere)
		class USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(Category = "Visual", VisibleDefaultsOnly)
		class UCameraComponent* FirstPersonCamera;

    UPROPERTY(Category = "Visual", VisibleAnywhere)
        class USceneComponent* GunLocation;

	class UInventoryComponent* WeaponInventory;

	float StoredSpeedBeforeJump;

	FTimerHandle SuperJumpTimer;
	FTimerHandle SlowDescentTimer;

private:
	void AttachNewWeapon(ABaseGun* nextGun);
};
