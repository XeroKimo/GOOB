// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminus_22XXGameModeBase.h"
#include "Actors/PlayerCharacter.h"
#include "Networked/NetPlayerCharacter.h"
#include "Networked/NetBaseGun.h"
#include "Networked/NetPlayerState.h"
#include "Terminus_22XX_GameState.h"
#include "Checkpoint.h"
#include "Guns/BaseGun.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ATerminus_22XXGameModeBase::ATerminus_22XXGameModeBase()
{

}

void ATerminus_22XXGameModeBase::SpawnStartingWeapons_Implementation(ANetPlayerCharacter * character)
{
	//Spawn starting weapons based on the game mode
	for (int i = 0; i < StartingWeapons.Num(); i++)
	{
        if (StartingWeapons[i] != NULL)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Instigator = character;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Owner = character;
            ANetBaseGun* weapon = GetWorld()->SpawnActor<ANetBaseGun>(StartingWeapons[i], SpawnParams); 
			//If the player already has the starting weapons, destroy
            if (character)
            {
                if (!character->AddWeaponToInventory(weapon))
                    weapon->Destroy();
            }
            else
            {
                weapon->Destroy();
            }
        }
	}
}

bool ATerminus_22XXGameModeBase::SpawnStartingWeapons_Validate(ANetPlayerCharacter * character)
{
    return true;
}

void ATerminus_22XXGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController * NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);


    //Comment the code below in order to spawn from Camera Location

	//Make sure a player is controlled
    if (!NewPlayer->GetPawn())
        RespawnPlayer(NewPlayer);

	//Set the initial spawn point of the player
    ANetPlayerCharacter* baseCharacter = Cast<ANetPlayerCharacter>(NewPlayer->GetPawn());
    int playerCount = Cast<ATerminus_22XX_GameState>(GameState)->ConnectedPlayers.Num();
    Cast<ANetPlayerState>(NewPlayer->GetPawn()->PlayerState)->StartingPointID = playerCount;
    Cast<ANetPlayerState>(NewPlayer->GetPawn()->PlayerState)->CurrentCheckpointID = playerCount;

	//Respawn player
    RespawnPlayer(NewPlayer, true); 
    baseCharacter = Cast<ANetPlayerCharacter>(NewPlayer->GetPawn());
    if (baseCharacter)
    {
		//Make the game state keep track of all logged in players
        baseCharacter->LogIn();
		//Spawn Starting weapons
        SpawnStartingWeapons(baseCharacter);
    }
    ///Comment up to here
}

void ATerminus_22XXGameModeBase::RespawnPlayer(APlayerController * NewPlayer, bool UseFirstCheckpoint)
{
    TArray<AActor*> PlayerStarts;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACheckpoint::StaticClass(), PlayerStarts);

    APawn* oldPawn = NewPlayer->GetPawn();
    APawn* pawn = nullptr;
    int checkpointID = 0;
    if (oldPawn)
    {
		//Spawn at first checkpoint
        if (UseFirstCheckpoint)
            checkpointID = Cast<ANetPlayerState>(oldPawn->PlayerState)->StartingPointID;
		//Spawn at current checkpoint
        else
            checkpointID = Cast<ANetPlayerState>(oldPawn->PlayerState)->CurrentCheckpointID;
    }
	//Search for the matching checkpoint and spawn there
    for (int i = 0; i < PlayerStarts.Num(); i++)
    {
        if (Cast<ACheckpoint>(PlayerStarts[i])->CheckpointID == checkpointID)
        {
            pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[i]);
            break;
        }
    }

    if (pawn)
    {
        if (ANetPlayerCharacter* baseCharacter = Cast<ANetPlayerCharacter>(pawn))
        {
			//Set the new pawn's player state to the old one   
            if (oldPawn != nullptr)
                baseCharacter->SetPlayerState(oldPawn->PlayerState);
			//Make the player controller control the newly spawned pawn
            NewPlayer->AcknowledgePossession(baseCharacter);
            NewPlayer->SetPawn(pawn);
			//Destroy old pawn
            if (oldPawn)
                oldPawn->Destroy();
            RestartPlayer(NewPlayer);
        }
    }
}
