// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShieldComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERMINUS_22XX_API UShieldComponent : public USceneComponent
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
		void ComponentHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void DecrementActiveGenerators();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDecrementActiveGenerators();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticastDisableShieldCollisions();
	UFUNCTION()
	void OnRep_Generators();

	virtual void EnableShieldCollisions();

	virtual void DisableShieldCollisions();

protected:
	UPROPERTY(Replicated)
	bool IsShieldActive;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* ShieldMesh;

	UPROPERTY(EditInstanceOnly, Category = "Shield")
		TArray<class AShieldGenerator*> ArrayOfShieldGenerators;

	UPROPERTY(ReplicatedUsing = OnRep_Generators)
	//UPROPERTY(Replicated)
		int ShieldGeneratorsAlive = 0;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		float MaxShieldHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		float CurrentShieldHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Shield")
		bool CanBeDestroyed = false;

	FCollisionResponseContainer EnabledCollisions;
};
