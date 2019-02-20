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
		class UCapsuleComponent* GeneratorHitBox;

	float GeneratorHealth = 1.0f;

	UPROPERTY(Category = "Defaults", EditInstanceOnly)
	bool GeneratorIsActive;

	TArray<class UShieldComponent*> PointersToShields;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void AddShield(class UShieldComponent* Shield);
	void GeneratorSheild();

	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	
	//Setters
	void SetGeneratorHealth(int health) { GeneratorHealth = health; }
	void SetGeneratorIsActive(bool state) { GeneratorIsActive = state; }
	
	//Getters
	int GetGeneratorHealth() { return GeneratorHealth; }
	bool GetGeneratorIsActive() { return GeneratorIsActive; }
	


	
	
};
