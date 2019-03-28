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
	for (int i = 0; i < StartingWeapons.Num(); i++)
	{
        if (StartingWeapons[i] != NULL)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Instigator = character;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Owner = character;
            ANetBaseGun* weapon = GetWorld()->SpawnActor<ANetBaseGun>(StartingWeapons[i], SpawnParams); 
			if (!character->AddWeaponToInventory(weapon))
				weapon->Destroy();
			//character->ServerAddWeaponToInvetory(weapon);
            //if (!character->PickupSuccess)
            //{
            //    weapon->Destroy();
            //}
            //else
            //{
            //    character->GetPlayerState()->CurrentGuns.Add(weapon);
            //    character->ServerResetPickupState();
            //}
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

    if (!NewPlayer->GetPawn())
        RespawnPlayer(NewPlayer);

    ANetPlayerCharacter* baseCharacter = Cast<ANetPlayerCharacter>(NewPlayer->GetPawn());
    int playerCount = Cast<ATerminus_22XX_GameState>(GameState)->ConnectedPlayers.Num();
    Cast<ANetPlayerState>(NewPlayer->GetPawn()->PlayerState)->StartingPointID = playerCount;

    RespawnPlayer(NewPlayer, true); 
    baseCharacter = Cast<ANetPlayerCharacter>(NewPlayer->GetPawn());
    if (baseCharacter)
    {
        baseCharacter->LogIn();
        SpawnStartingWeapons(baseCharacter);
    }

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
        if (UseFirstCheckpoint)
            checkpointID = Cast<ANetPlayerState>(oldPawn->PlayerState)->StartingPointID;
        else
            checkpointID = Cast<ANetPlayerState>(oldPawn->PlayerState)->CurrentCheckpointID;
    }
    for (int i = 0; i < PlayerStarts.Num(); i++)
    {
        if (Cast<ACheckpoint>(PlayerStarts[i])->CheckpointID == checkpointID)
        {
            pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[i]);
        }
    }

    if (pawn)
    {
        if (ANetPlayerCharacter* baseCharacter = Cast<ANetPlayerCharacter>(pawn))
        {
            
            if (oldPawn != nullptr)
                baseCharacter->SetPlayerState(oldPawn->PlayerState);
            NewPlayer->AcknowledgePossession(baseCharacter);
            //NewPlayer->SetPawn(pawn);
            if (oldPawn)
                oldPawn->Destroy();
            RestartPlayer(NewPlayer);
        }
    }
}
