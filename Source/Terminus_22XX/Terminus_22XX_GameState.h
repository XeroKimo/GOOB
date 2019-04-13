// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Terminus_22XX_GameState.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ATerminus_22XX_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
    ATerminus_22XX_GameState();

    void Tick(float deltaTime) override;

protected:
	void HandleMatchHasEnded() override;
public:
    UPROPERTY(BlueprintReadOnly, Replicated)
        TArray<APlayerState*> ConnectedPlayers;
	UPROPERTY(BlueprintReadOnly, Replicated)
		TArray<bool> PlayerReachedBoss;
    UPROPERTY(BlueprintReadOnly, Replicated)
        float CurrentGameTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Replicated)
		bool StartBossCountDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float BossCountdown = 45.0f;
};
