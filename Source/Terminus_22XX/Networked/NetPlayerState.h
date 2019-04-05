// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly, Replicated)
        int CurrentCheckpointID;
	
    UPROPERTY(BlueprintReadOnly, Replicated)
        float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float MaxHealth;

    UPROPERTY(Replicated)
        TArray<class ANetBaseGun*> CurrentGuns;
	
    UPROPERTY(Replicated)
		int StartingPointID;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int PlayerScore;


public:
	UFUNCTION(BlueprintCallable)
		float GetPercentHealth();
};
