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
	
	void ForceStopAndSlowDescent();
private:
	void StoreCurrentSpeed();

public:
	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseMovementSpeed;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseTurnSpeed;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float JumpForce = 12000.f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float BaseSuperJumpMultiplier = 0.8f;

	UPROPERTY(Category = "Movement", EditAnywhere)
		float AddedSuperJumpMultiplier = 0.7f;

	UPROPERTY(Category = "Status", EditAnywhere)
		float MaxHealth;

	UPROPERTY(Category = "Weapon", EditAnywhere)
		class AActor* Weapon;

	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float ReleaseSuperJumpTime = 2.0f;

	UPROPERTY(Category = "MovementTimer", EditAnywhere)
		float SuperJumpTimerDelay = 0.1f;

	float CurrentHealth;

private:
	UPROPERTY(Category = "Visual", VisibleDefaultsOnly)
		class USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(Category = "Visual", VisibleDefaultsOnly)
		class UCameraComponent* FirstPersonCamera;

	float StoredSpeedBeforeJump;

	FTimerHandle SuperJumpTimer;
	FTimerHandle SlowDescentTimer;
};
