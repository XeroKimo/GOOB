// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "Guns/BaseGun.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int i = 0; i < EGunName::GN_Max; i++)
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
	for (int i = 0; i < GN_Max; i++)
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

ABaseGun * UInventoryComponent::SwitchToGun(int index)
{
	SwitchWeapon(index);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetShotgun()
{
	SwitchWeapon(EGunName::GN_Shotgun);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetVampyr()
{
	SwitchWeapon(EGunName::GN_Vampyr);
	return Weapons[CurrentWeaponIndex];
}

ABaseGun * UInventoryComponent::GetRailgun()
{
	SwitchWeapon(EGunName::GN_Railgun);
	return Weapons[CurrentWeaponIndex];
}

bool UInventoryComponent::AddWeapon(ABaseGun * AGun)
{
	if (AGun)
	{
		int gunIndex = AGun->GetWeaponIndex();
		bool exists = WeaponExists(gunIndex);
		Weapons[gunIndex] = (exists) ? Weapons[gunIndex] : AGun;
		return !exists;
	}
	return false;
}

void UInventoryComponent::SwitchWeapon(int newIndex)
{
	int oldWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex = newIndex;
	if (!WeaponExists(CurrentWeaponIndex))
		CurrentWeaponIndex = oldWeaponIndex;
}

