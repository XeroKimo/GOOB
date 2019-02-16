// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

UCLASS()
class TERMINUS_22XX_API ABaseBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBullet();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(Category = "Movement", VisibleAnywhere)
		class UProjectileMovementComponent* MovementComponent;

	UPROPERTY(Category = "Bullet", BlueprintReadOnly)
		float BulletDamage;

protected:
	UPROPERTY(Category = "Visual", VisibleAnywhere)
		class UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(Category = "Hitbox", VisibleAnywhere)
		class UCapsuleComponent* BulletHitbox;

	UPROPERTY(Category = "Bullet", EditAnywhere)
		float BulletSpeed;

public:
	void SetBulletDamage(float NewDamage) { BulletDamage = NewDamage; }
	void SetBulletDirection(FVector Direction);
protected:
	UFUNCTION()
		void ComponentHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
