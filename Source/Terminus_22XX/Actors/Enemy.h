// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

UCLASS()
class TERMINUS_22XX_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

    

private:
    UPROPERTY(Category = "Status", EditAnywhere)
        float EnemyHealth = 0;

	UPROPERTY(Category = "Score", EditAnywhere)
		float ScoreValue = 2.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
        class UStaticMeshComponent* EnemyMesh;

    UPROPERTY(VisibleAnywhere, Category = "HitBox")
        class UCapsuleComponent* HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		class UCapsuleShieldComponent* Shield;

    UPROPERTY(EditInstanceOnly, Category = "AI")
        bool bPatrol;

   
    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
        AActor* FirstPatrolPoint;

   
    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
        AActor* SecondPatrolPoint;

    
     
    AActor* CurrentPatrolPoint;

   

public:	

    bool bCanBeDamage = false;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	// Called to bind functionality to input
    void SetEmenyHealth(float health) { EnemyHealth = health;}
	
	UFUNCTION()
		void MoveToNextPatrolPoint();
	
};
