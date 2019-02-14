// Fill out your copyright notice in the Description page of Project Settings.

#include "T22XX_Shotgun.h"

AT22XX_Shotgun::AT22XX_Shotgun()
{
	MaxClipSize = 6;
	CurrentClipSize = CurrentClipSize;
	InfiniteClipSize = true;
	MaxStockAmmo = 120;
	ReloadSpeed = 1.0f;
	FireRate = 30;
	PelletsPerShot = 12;
}


void AT22XX_Shotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AT22XX_Shotgun::Tick(float DeltaTime)
{
	ABaseGun::Tick(DeltaTime);

}
