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
    //The checkpoint which the player respawns to
    UPROPERTY(BlueprintReadOnly, Replicated)
        int CurrentCheckpointID;
	//The current health of the player
    UPROPERTY(BlueprintReadOnly, Replicated)
        float CurrentHealth;
    //The guns which the player holds
    UPROPERTY(Replicated)
        TArray<class ANetBaseGun*> CurrentGuns;
    //The initial spawn point of a player
    int StartingPointID;
    //The max health of the player
	UPROPERTY(BlueprintReadOnly, Replicated)
		float MaxHealth;
    //The player score
	UPROPERTY(BlueprintReadOnly, Replicated)
		int PlayerScore = 0;
    //Return a %value of the player's remaining health
	UFUNCTION(BlueprintCallable)
		float GetRemainingHealth();
};
