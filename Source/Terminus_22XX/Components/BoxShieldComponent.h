// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShieldComponent.h"
#include "BoxShieldComponent.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API UBoxShieldComponent : public UShieldComponent
{
	GENERATED_BODY()
	
	
protected:
	UPROPERTY(EditInstanceOnly, Category = "Collision")
		class UBoxComponent* BoxCollider;
	
public:
	UBoxShieldComponent();

	virtual void EnableShieldCollisions();

	virtual void DisableShieldCollisions();
protected:
	void BeginPlay() override;
};
