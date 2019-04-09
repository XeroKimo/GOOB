// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerState.h"
#include "Net/UnrealNetwork.h"


float ANetPlayerState::GetRemainingHealth()
{
    //return the %current health of the player
	return CurrentHealth / MaxHealth;
}

void ANetPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    //DOREPLIFETIME all replicated values
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANetPlayerState, CurrentCheckpointID);
    DOREPLIFETIME(ANetPlayerState, CurrentGuns);
    DOREPLIFETIME(ANetPlayerState, CurrentHealth);
	DOREPLIFETIME(ANetPlayerState, MaxHealth);
	DOREPLIFETIME(ANetPlayerState, PlayerScore);
}