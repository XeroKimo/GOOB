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

    void BeginPlay() override;
    void Tick(float deltaTime) override;
	
private:
    void UpdateCurrentTime();

public:
    UPROPERTY(BlueprintReadOnly, Replicated)
        TArray<APlayerState*> ConnectedPlayers;
    UPROPERTY(BlueprintReadOnly, Replicated)
        float CurrentGameTime = 0.0f;

};
