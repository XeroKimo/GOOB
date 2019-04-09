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

    UPROPERTY(Replicated)
        TArray<class ANetBaseGun*> CurrentGuns;

    int StartingPointID;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int PlayerScore = 0;

	UFUNCTION(BlueprintCallable)
		float GetRemainingHealth();
};
