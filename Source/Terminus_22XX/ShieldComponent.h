// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShieldComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERMINUS_22XX_API UShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	virtual void SetupAttachment(USceneComponent* scene);

protected:
	bool IsShieldActive;
	class UStaticMeshComponent* ShieldMesh;

	//UPROPERTY(VisisbleAnywhere, Category = "Shield")
	//class AShieldGenerator* ShieldGenerator;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		float MaxShieldHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		float CurrentShieldHealth = 100.f;
		
	UPROPERTY(VisibleAnywhere, Category = "Shield")
		bool CanBeDestroyed = false;
	
};
