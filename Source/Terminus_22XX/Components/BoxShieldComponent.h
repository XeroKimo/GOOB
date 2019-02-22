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
	
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
		class UBoxComponent* BoxCollider;
	
public:
	UBoxShieldComponent();
	virtual void SetupAttachment(USceneComponent* Scene, FName SocketName = NAME_None) override;
	class UBoxComponent* GetBox() { return BoxCollider; }
};
