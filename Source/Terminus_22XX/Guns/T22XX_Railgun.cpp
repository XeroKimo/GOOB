// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Railgun.h"

AT22XX_Railgun::AT22XX_Railgun()
{
	CurrentAmmoCount = MaxAmmoCount = 100;
	InfiniteClip = true;
	MaxAmmoStock = 0;
	ReloadRate = 2.0f;
	FireRate = 30;
	PelletCount = 1;

	WeaponIndex = EGunName::GN_Railgun;
	Tags.Add("Railgun");
}