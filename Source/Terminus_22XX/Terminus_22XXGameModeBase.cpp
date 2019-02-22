// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminus_22XXGameModeBase.h"
#include "Actors/PlayerCharacter.h"
#include "Guns/BaseGun.h"
#include "Engine/World.h"

ATerminus_22XXGameModeBase::ATerminus_22XXGameModeBase()
{

}

void ATerminus_22XXGameModeBase::SpawnStartingWeapons(APlayerCharacter * character)
{
	for (int i = 0; i < StartingWeapons.Num(); i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = character;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = character;
		ABaseGun* weapon = GetWorld()->SpawnActor<ABaseGun>(StartingWeapons[i], SpawnParams);
		if (!character->AddWeaponToInvetory(weapon))
		{
			weapon->Destroy();
		}
	}
}
