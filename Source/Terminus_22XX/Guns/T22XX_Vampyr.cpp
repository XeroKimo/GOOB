// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Vampyr.h"

//Default values for the vampyr
AT22XX_Vampyr::AT22XX_Vampyr()
{
	FireStyle = EFireStyle::FS_FullAuto;
	CurrentAmmoCount = MaxAmmoCount = 50;
	InfiniteClip = true;
	MaxAmmoStock = 1000;
	ReloadRate = 1.0f;
	FireRate = 360;
	PelletCount = 1;

	WeaponIndex = EGunName::GN_Vampyr;
	Tags.Add("Vampyr");
}


