// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Shotgun.h"

AT22XX_Shotgun::AT22XX_Shotgun()
{
	CurrentClipSize = MaxClipSize = 6;
	InfiniteClipSize = true;
	MaxStockAmmo = 120;
	ReloadSpeed = 1.0f;
	FireRate = 30;
	PelletsPerShot = 12;

	WeaponIndex = EGunName::GN_Shotgun;
	Tags.Add("Shotgun");
}
