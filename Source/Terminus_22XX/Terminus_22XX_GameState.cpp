// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminus_22XX_GameState.h"
#include "Net/UnrealNetwork.h"
#include "Public/TimerManager.h"
#include "GameFramework/GameMode.h"
#include "Networked/NetPlayerState.h"
#include "Engine/World.h"

ATerminus_22XX_GameState::ATerminus_22XX_GameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATerminus_22XX_GameState::Tick(float deltaTime)
{
	if (Role == ROLE_Authority)
	{
		//Update time pass since game start
		if (IsMatchInProgress())
			CurrentGameTime += deltaTime;
		//Start the final count down for the game to end
		if (StartBossCountDown)
			BossCountdown -= deltaTime;
		//End the game when boss count down has reached 0
		if (BossCountdown <= 0.f)
		{
			Cast<AGameMode>(GetWorld()->GetAuthGameMode())->EndMatch();
		}
	}

}

void ATerminus_22XX_GameState::HandleMatchHasEnded()
{
	for (int i = 0; i < ConnectedPlayers.Num(); i++)
	{
		//Calculate the final time for each player by subtracting score from time
        if (PlayerReachedBoss[i])
        {
            ANetPlayerState* playerState = Cast < ANetPlayerState>(ConnectedPlayers[i]);
            playerState->TimeWhenBossReached -= playerState->Score;
        }
        else
        {
            PlayerReachedBoss[i] = true;
            ANetPlayerState* playerState = Cast < ANetPlayerState>(ConnectedPlayers[i]);
            playerState->TimeWhenBossReached = CurrentGameTime - playerState->Score;
        }
	}
}

void ATerminus_22XX_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATerminus_22XX_GameState, ConnectedPlayers);
	DOREPLIFETIME(ATerminus_22XX_GameState, PlayerReachedBoss);
	DOREPLIFETIME(ATerminus_22XX_GameState, CurrentGameTime);
	DOREPLIFETIME(ATerminus_22XX_GameState, StartBossCountDown);
	DOREPLIFETIME(ATerminus_22XX_GameState, BossCountdown);
}