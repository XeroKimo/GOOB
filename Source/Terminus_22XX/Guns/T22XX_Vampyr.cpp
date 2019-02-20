// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Vampyr.h"

AT22XX_Vampyr::AT22XX_Vampyr()
{
	ShootingStyle = EFireStyle::FS_FullAuto;
	CurrentClipSize = MaxClipSize = 50;
	InfiniteClipSize = true;
	MaxStockAmmo = 1000;
	ReloadSpeed = 1.0f;
	FireRate = 360;
	PelletsPerShot = 1;

	WeaponIndex = EGunName::GN_Vampyr;
	Tags.Add("Vampyr");
}


