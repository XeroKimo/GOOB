// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetPlayerCharacter.generated.h"

enum MovementStatus
{
	MovementStatus_Idle = 0x0001,
	MovementStatus_Walking = 0x0002,
	MovementStatus_Jumping = 0x0004,
};

UCLASS()
class TERMINUS_22XX_API ANetPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetPlayerCharacter();
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
	void PrepareSuperJump();
	void ReleaseSuperJump();

	void FireWeapon();
	void StopFireWeapon();
	void Reload();

	void SwitchWeapon(float Val);
	void NextWeapon();
	void PreviousWeapon();
	void SwitchToShotgun();
	void SwitchToVampyr();
	void SwitchToRailgun();

    void LogIn();
    void Respawn();
    void SetPlayerState(class APlayerState* state);
	void SetActiveCheckpoint(int CheckpointID);

    class ANetPlayerState* GetPlayerState();
    class ATerminus_22XX_GameState* GetGameState();

    //UFUNCTION(Client,Reliable)
    //bool AddWeaponToInventory(class ANetBaseGun* AGun);

	bool AddHealth(float Amount);
	bool AddWeaponToInventory(class ANetBaseGun* AGun);
	//UFUNCTION(BlueprintCallable, Category = "Status")
	//	float GetHealthPercentage();

	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	class USceneComponent* GetGunScene() { return GunLocation; }
private:
	void StoreCurrentSpeed();

public:
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxWalkSpeed = 1250.0f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseTurnSpeed = 50.0f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxAcceleration = 1000.f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float JumpZVelocity = 1000.f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseSuperJumpMultiplier = 0.3f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float AddedSuperJumpMultiplier = 0.7f;

	//The normal Gravity scale
	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseGravityScale = 2.0f;

	//The Gravity Scale when player prepares a super jump
	UPROPERTY(Category = "Movement", EditAnywhere)
		float WeakenedGravityScale = 0.05f;

	//This is a multiplier, taking the power being JumpVelocity * JumpForce * MaxSuperJumpPowerScale
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxSuperJumpPowerScale = 2.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Status", EditAnywhere)
		float MaxHealth;

	//How much time before the super jump auto releases
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float ReleaseSuperJumpTime = 1.0f;

	//How much time before the super jump starts preparing
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float SuperJumpTimerDelay = 0.15f;

	//How much time it takes for super jump to reach max power
	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float MaxPowerSuperJumpTime = 0.5f;

	///UPROPERTY(BlueprintReadWrite, Category = "Status", EditAnywhere)
	///	float CurrentHealth;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		bool EnableWeaponScrolling = true;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		float WeaponSwitchLockout = 0.125f;

    UPROPERTY(Category = "Status", EditAnywhere)
        float RespawnDelay = 1.0f;
private:
	UPROPERTY(Category = "Visual", VisibleDefaultsOnly)
		class UCameraComponent* FirstPersonCamera;

	UPROPERTY(Category = "Visual", VisibleAnywhere)
		class USceneComponent* GunLocation;

	UPROPERTY(VisibleAnywhere)
		class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere)
		class USoundBase* WalkingSound;
	UPROPERTY(EditAnywhere)
		class USoundBase* JumpingSound;
	UPROPERTY(EditAnywhere)
		class USoundBase* LandingSound;
	UPROPERTY(EditAnywhere)
		class USoundBase* ChargingSound;
	UPROPERTY(EditAnywhere)
		class USoundBase* ReleaseSuperJumpSound;

	FTimerHandle SuperJumpTimer;
	FTimerHandle SlowDescentTimer;
	FTimerHandle WeaponSwitchTimer;

protected:
	class UNetInventoryComponent* WeaponInventory;

	TEnumAsByte<MovementStatus> CurrentStatus;
	TEnumAsByte<MovementStatus> PreviousStatus;

public:
	UPROPERTY(Category = "Weapon", VisibleInstanceOnly, Replicated)
		class ANetBaseGun* CurrentWeapon = nullptr;

private:
	UPROPERTY(Replicated)
		float StoredSpeedBeforeJump;

	//delete later
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerCameraDebug(FRotator rot);
public:

	void NormalDescent();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticastNormalDescent();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticastForceStopAndSlowDescent();

    UFUNCTION(NetMulticast, Reliable)
        void NetMulticastPlaySound(class USoundBase* soundClip);

    UFUNCTION(Server, Reliable, WithValidation)
        void ServerPlaySound(class USoundBase* soundClip);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNormalDescent();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSuperJump(FVector Impulse);

	UFUNCTION(Server,Reliable, WithValidation)
		void ServerForceStopAndSlowDescent();

    UFUNCTION(Server, Reliable, WithValidation)
        void ServerRespawn();

private:
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerAttachNewWeapon(class ANetBaseGun* nextGun);

	
	
};
