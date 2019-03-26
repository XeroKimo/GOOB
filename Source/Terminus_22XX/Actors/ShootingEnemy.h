// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Enemy.h"
#include "ShootingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API AShootingEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:
	AShootingEnemy();

	void Tick(float DeltaTime);
	void DetectPlayer();
	void ShootPlayer();
protected:
	void BeginPlay() override;

	void ResetShoot();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Hitbox")
		class USphereComponent* PlayerDetection;
	UPROPERTY(VisibleAnywhere, Category = "Hitbox")
		class UArrowComponent* ShootingArrow;
	UPROPERTY(EditAnywhere, Category = "AI")
		TSubclassOf<class ABaseBullet> BulletClass;
	UPROPERTY(EditAnywhere, Category = "AI")
		int Damage = 10.0f;
	UPROPERTY(EditAnywhere, Category = "AI")
		float RotationSpeed = 90.0f;

	float TimeBetweenShots = 0.25f;
	bool CanShoot = true;
	FTimerHandle ShootingTimer;
	
private:
	bool WasPatrolling = false;
};
