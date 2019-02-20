// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "BaseGun.h"
#include "T22XX_Shotgun.h"
#include "T22XX_Vampyr.h"
#include "T22XX_Railgun.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int i = 0; i < 3; i++)
	{
		ABaseGun* gun = nullptr;
		Weapons.Push(gun);
	}
	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

ABaseGun * UInventoryComponent::GetAWeapon()
{
	for (int i = 0; i < 3; i++)
	{
		if (WeaponExists(i))
		{
			CurrentWeaponIndex = i;
			return Weapons[i];
		}
	}
	return nullptr;
}

ABaseGun * UInventoryComponent::NextWeapon()
{
	SwitchWeapon((CurrentWeaponIndex + 1) % 3);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::PreviousWeapon()
{
	SwitchWeapon((CurrentWeaponIndex - 1 < 0) ? 2 : CurrentWeaponIndex - 1);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetShotgun()
{
	SwitchWeapon(0);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetVampyr()
{
	SwitchWeapon(1);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetRailgun()
{
	SwitchWeapon(2);
	return Weapons[CurrentWeaponIndex];
}

void UInventoryComponent::AddWeapon(ABaseGun * AGun)
{
	if (AGun)
	{
		int gunIndex = AGun->GetWeaponIndex();
		Weapons[gunIndex] = (WeaponExists(gunIndex)) ? Weapons[gunIndex] : AGun;
	}
}

void UInventoryComponent::SwitchWeapon(int newIndex)
{
	int oldWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex = newIndex;
	if (!WeaponExists(CurrentWeaponIndex))
		CurrentWeaponIndex = oldWeaponIndex;
}

