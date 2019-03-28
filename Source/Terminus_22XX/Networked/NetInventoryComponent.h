// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetInventoryComponent.generated.h"

class ANetBaseGun;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERMINUS_22XX_API UNetInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
    UNetInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	ANetBaseGun* GetAWeapon();

	ANetBaseGun* NextWeapon();
	ANetBaseGun* PreviousWeapon();
	ANetBaseGun* SwitchToGun(int index);

	ANetBaseGun* GetShotgun();
	ANetBaseGun* GetVampyr();
	ANetBaseGun* GetRailgun();

	bool AddWeapon(ANetBaseGun* AGun);

	ANetBaseGun* GetCurrentWeapon() { return Weapons[CurrentWeaponIndex]; }
private:
	void SwitchWeapon(int newIndex);
	bool WeaponExists(int index) { return Weapons[index] != nullptr; }

private:
    UPROPERTY(Replicated)
	TArray<ANetBaseGun*> Weapons;
    UPROPERTY(Replicated)
	int CurrentWeaponIndex = 0;
	
};
