// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerState.h"
#include "Net/UnrealNetwork.h"


float ANetPlayerState::GetPercentHealth()
{
	return CurrentHealth / MaxHealth;
}


void ANetPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetPlayerState, CurrentCheckpointID);
	DOREPLIFETIME(ANetPlayerState, CurrentGuns);
	DOREPLIFETIME(ANetPlayerState, CurrentHealth);
	DOREPLIFETIME(ANetPlayerState, MaxHealth);
	DOREPLIFETIME(ANetPlayerState, StartingPointID);
	DOREPLIFETIME(ANetPlayerState, PlayerScore);
}
