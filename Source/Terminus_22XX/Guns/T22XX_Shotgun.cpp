// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Shotgun.h"

AT22XX_Shotgun::AT22XX_Shotgun()
{
	CurrentAmmoCount = MaxAmmoCount = 6;
	InfiniteClip = true;
	MaxAmmoStock = 120;
	ReloadRate = 1.0f;
	FireRate = 30;
    PelletCount = 12;
	WeaponIndex = EGunName::GN_Shotgun;
	Tags.Add("Shotgun");
}
