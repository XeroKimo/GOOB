// Fill out your copyright notice in the Description page of Project Settings.

#include "NetInventoryComponent.h"
#include "NetBaseGun.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UNetInventoryComponent::UNetInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	for (int i = 0; i < EGunName::GN_Max; i++)
	{
		ANetBaseGun* gun = nullptr;
		Weapons.Push(gun);
	}

    SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UNetInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNetInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

ANetBaseGun * UNetInventoryComponent::GetAWeapon()
{
	//Return the first weapon that exists
	//In the invetory
	for (int i = 0; i < GN_Max; i++)
	{
		if (WeaponExists(i))
		{
			CurrentWeaponIndex = i;
			return Weapons[i];
		}
	}
	//If we can't find one, return null
	return nullptr;
}

ANetBaseGun * UNetInventoryComponent::NextWeapon()
{
	SwitchWeapon((CurrentWeaponIndex + 1) % GN_Max);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::PreviousWeapon()
{
	SwitchWeapon((CurrentWeaponIndex - 1 < 0) ? GN_Max - 1 : CurrentWeaponIndex - 1);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::SwitchToGun(int index)
{
	SwitchWeapon(index);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::GetShotgun()
{
	SwitchWeapon(EGunName::GN_Shotgun);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::GetVampyr()
{
	SwitchWeapon(EGunName::GN_Vampyr);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::GetRailgun()
{
	SwitchWeapon(EGunName::GN_Railgun);
	return Weapons[CurrentWeaponIndex];
}

bool UNetInventoryComponent::AddWeapon(ANetBaseGun * AGun)
{
	//Only add weapons if we are in role authority
	if (GetOwnerRole() == ROLE_Authority)
	{
		//Check if the gun isn't null
		if (AGun)
		{
			//Get the gun's weapon index
			int gunIndex = AGun->GetWeaponIndex();
			//See if it exists
			bool exists = WeaponExists(gunIndex);
			//If it exists, do nothing, else add it to the inventory
			Weapons[gunIndex] = (exists) ? Weapons[gunIndex] : AGun;
			//return Not Exists
			return !exists;
		}
	}
    return false;
}

void UNetInventoryComponent::SwitchWeapon(int newIndex)
{
	//Check if the weapon exists before switching gunss
	if (WeaponExists(newIndex))
		CurrentWeaponIndex = newIndex;
}

bool UNetInventoryComponent::WeaponExists(int index)
{
	//Sees if weapons exists
	return Weapons[index] != nullptr; 
}

void UNetInventoryComponent::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UNetInventoryComponent, CurrentWeaponIndex);
    DOREPLIFETIME(UNetInventoryComponent, Weapons);
}