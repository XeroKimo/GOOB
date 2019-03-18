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

ANetBaseGun * UNetInventoryComponent::NextWeapon()
{
	SwitchWeapon((CurrentWeaponIndex + 1) % 3);
	return Weapons[CurrentWeaponIndex];
}

ANetBaseGun * UNetInventoryComponent::PreviousWeapon()
{
	SwitchWeapon((CurrentWeaponIndex - 1 < 0) ? 2 : CurrentWeaponIndex - 1);
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
	if (AGun)
	{
		int gunIndex = AGun->GetWeaponIndex();
		bool exists = WeaponExists(gunIndex);
		Weapons[gunIndex] = (exists) ? Weapons[gunIndex] : AGun;
		return !exists;
	}
	return false;
}

void UNetInventoryComponent::SwitchWeapon(int newIndex)
{
	int oldWeaponIndex = CurrentWeaponIndex;
	CurrentWeaponIndex = newIndex;
	if (!WeaponExists(CurrentWeaponIndex))
		CurrentWeaponIndex = oldWeaponIndex;
}

void UNetInventoryComponent::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UNetInventoryComponent, CurrentWeaponIndex);
    DOREPLIFETIME(UNetInventoryComponent, Weapons);
}