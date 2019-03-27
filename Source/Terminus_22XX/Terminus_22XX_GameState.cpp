// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminus_22XX_GameState.h"
#include "Net/UnrealNetwork.h"

ATerminus_22XX_GameState::ATerminus_22XX_GameState()
{
    //PrimaryActorTick.bCanEverTick = true;
}

void ATerminus_22XX_GameState::BeginPlay()
{
    FTimerHandle updateTime;
    if (Role == ROLE_Authority)
        GetWorldTimerManager().SetTimer(updateTime, this, &ATerminus_22XX_GameState::UpdateCurrentTime, 1.0f, true);
}

void ATerminus_22XX_GameState::Tick(float deltaTime)
{
    if (Role == ROLE_Authority)
        CurrentGameTime += deltaTime;
}

void ATerminus_22XX_GameState::UpdateCurrentTime()
{
    CurrentGameTime += 1.0f;
}

void ATerminus_22XX_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATerminus_22XX_GameState, ConnectedPlayers);
    DOREPLIFETIME(ATerminus_22XX_GameState, CurrentGameTime);
}