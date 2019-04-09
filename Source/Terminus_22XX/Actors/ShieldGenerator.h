// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldGenerator.generated.h"

UCLASS()
class TERMINUS_22XX_API AShieldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldGenerator();

	//TArray<AShield> NumbersOfShields
private:
	UPROPERTY(Category = "Mesh", EditAnywhere)
		class UStaticMeshComponent* GeneratorMesh;

	UPROPERTY(Category = "Hitbox", EditAnywhere)
		class UBoxComponent* GeneratorHitBox;

	UPROPERTY(Category = "Defaults", EditAnywhere, Replicated)
		float GeneratorHealth = 300.0f;

	UPROPERTY(Category = "Defaults", EditInstanceOnly, Replicated)
	bool GeneratorIsActive;

    UPROPERTY(Category = "Defaults", EditAnywhere, Replicated)
        class UMaterialInterface* AliveMaterial;

    UPROPERTY(Category = "Defaults", EditAnywhere, Replicated)
        class UMaterialInterface* DeadMaterial;


	TArray<class UShieldComponent*> PointersToShields;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
    
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void AddShield(class UShieldComponent* Shield);

	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//Getters
	bool GetGeneratorIsActive() { return GeneratorIsActive; }
	
private:
    UFUNCTION(NetMulticast,Reliable)
    void NetMulicastChangeMaterial(class UMaterialInterface* material);
	
	
};
