// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminus_22XXGameModeBase.h"
#include "Actors/PlayerCharacter.h"
#include "Networked/NetPlayerCharacter.h"
#include "Networked/NetBaseGun.h"
#include "Guns/BaseGun.h"
#include "Engine/World.h"

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
            character->ServerAddWeaponToInvetory(weapon);
            if (!character->PickupSuccess)
            {
                weapon->Destroy();
            }
            else
            {
                character->ServerResetPickupState();
            }
        }
	}
}

bool ATerminus_22XXGameModeBase::SpawnStartingWeapons_Validate(ANetPlayerCharacter * character)
{
    return true;
}
