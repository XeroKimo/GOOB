// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Railgun.h"

AT22XX_Railgun::AT22XX_Railgun()
{
	CurrentClipSize = MaxClipSize = 100;
	InfiniteClipSize = true;
	MaxStockAmmo = 0;
	ReloadSpeed = 2.0f;
	FireRate = 30;
	PelletsPerShot = 1;

	WeaponIndex = EGunName::GN_Railgun;
	Tags.Add("Railgun");
}