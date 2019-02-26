// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShieldComponent.h"
#include "SphereShieldComponent.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API USphereShieldComponent : public UShieldComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
		class USphereComponent* SphereCollider;
	
public:
	USphereShieldComponent();

	virtual void EnableShieldCollisions();

	virtual void DisableShieldCollisions();
protected:
	void BeginPlay() override;
};
