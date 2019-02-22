// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Terminus_22XXGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ATerminus_22XXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATerminus_22XXGameModeBase();

	void SpawnStartingWeapons(class APlayerCharacter* character);
private:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ABaseGun>> StartingWeapons;
	
	
};
