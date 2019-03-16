// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void AcknowledgePossession(class APawn* InPawn) override;

protected:
	void SetupInputComponent() override;
	
public:
	void MoveForeward(float Val);
	void MoveSideways(float Val);
	void LookUp(float Val);
	void LookSideways(float Val);

	void Jump();
	void StopJumping();
	void FireWeapon();
	void StopFireWeapon();
	void Reload();

	void SwitchWeapon(float Val);
	void NextWeapon();
	void PreviousWeapon();
	void SwitchToShotgun();
	void SwitchToVampyr();
	void SwitchToRailgun();
	
private:
	class ANetPlayerCharacter* character;
};
