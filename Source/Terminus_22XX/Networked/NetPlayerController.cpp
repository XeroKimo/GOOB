// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerController.h"
#include "Networked/NetPlayerCharacter.h"

void ANetPlayerController::AcknowledgePossession(class APawn* InPawn)
{
	APlayerController::AcknowledgePossession(InPawn);
	character = Cast<ANetPlayerCharacter>(InPawn);

}

void ANetPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForeward", this, &ANetPlayerController::MoveForeward);
	InputComponent->BindAxis("MoveSideways", this, &ANetPlayerController::MoveSideways);

	InputComponent->BindAxis("TurnUp", this, &ANetPlayerController::LookUp);
	InputComponent->BindAxis("TurnSide", this, &ANetPlayerController::LookSideways);

	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ANetPlayerController::Jump);
	InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ANetPlayerController::StopJumping);

	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ANetPlayerController::FireWeapon);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ANetPlayerController::StopFireWeapon);

	//InputComponent->BindAxis("SwitchWeapon", this, &ANetPlayerController::SwitchWeapon);
	//InputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &ANetPlayerController::NextWeapon);
	//InputComponent->BindAction("PreviousWeapon", EInputEvent::IE_Pressed, this, &ANetPlayerController::PreviousWeapon);
	//InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ANetPlayerController::Reload);
	//InputComponent->BindAction("Shotgun", EInputEvent::IE_Pressed, this, &ANetPlayerController::SwitchToShotgun);
	//InputComponent->BindAction("Vampyr", EInputEvent::IE_Pressed, this, &ANetPlayerController::SwitchToVampyr);
	//InputComponent->BindAction("Railgun", EInputEvent::IE_Pressed, this, &ANetPlayerController::SwitchToRailgun);
}

void ANetPlayerController::MoveForeward(float Val)
{
	if (character)
	{
		character->MoveForeward(Val);
	}
}

void ANetPlayerController::MoveSideways(float Val)
{
	if (character)
	{
		character->MoveSideways(Val);
	}
}

void ANetPlayerController::LookUp(float Val)
{
	if (character)
	{
		character->LookUp(Val);
	}
}

void ANetPlayerController::LookSideways(float Val)
{
	if (character)
	{
		character->LookSideways(Val);
	}
}

void ANetPlayerController::Jump()
{
	if (character)
	{
		character->Jump();
	}
}

void ANetPlayerController::StopJumping()
{
	if (character)
	{
		character->StopJumping();
	}
}
//
void ANetPlayerController::FireWeapon()
{
	if (character)
	{
		character->FireWeapon();
	}
}

void ANetPlayerController::StopFireWeapon()
{
	if (character)
	{
		character->StopFireWeapon();
	}
}
//
//void ANetPlayerController::Reload()
//{
//	if (character)
//	{
//		character->Reload();
//	}
//}
//
//void ANetPlayerController::SwitchWeapon(float Val)
//{
//	if (character)
//	{
//		character->SwitchWeapon(Val);
//	}
//}
//
//void ANetPlayerController::NextWeapon()
//{
//	if (character)
//	{
//		character->NextWeapon();
//	}
//}
//
//void ANetPlayerController::PreviousWeapon()
//{
//	if (character)
//	{
//		character->PreviousWeapon();
//	}
//}
//
//void ANetPlayerController::SwitchToShotgun()
//{
//	if (character)
//	{
//		character->SwitchToShotgun();
//	}
//}
//
//void ANetPlayerController::SwitchToVampyr()
//{
//	if (character)
//	{
//		character->SwitchToVampyr();
//	}
//}
//
//void ANetPlayerController::SwitchToRailgun()
//{
//	if (character)
//	{
//		character->SwitchToRailgun();
//	}
//}
