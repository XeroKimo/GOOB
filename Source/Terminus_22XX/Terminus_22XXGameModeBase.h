// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Terminus_22XXGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ATerminus_22XXGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATerminus_22XXGameModeBase();
    void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
    void RespawnPlayer(APlayerController* NewPlayer, bool UseFirstCheckpoint = false);

    UFUNCTION(Server, Reliable, WithValidation)
    void SpawnStartingWeapons(class ANetPlayerCharacter* character);
    //void SpawnStartingWeapons(class APlayerCharacter* character);
private:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ANetBaseGun>> StartingWeapons;
	
	
};
