// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseGun;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERMINUS_22XX_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	ABaseGun* GetAWeapon();

	ABaseGun* NextWeapon();
	ABaseGun* PreviousWeapon();
	ABaseGun* SwitchToGun(int index);

	ABaseGun* GetShotgun();
	ABaseGun* GetVampyr();
	ABaseGun* GetRailgun();

	bool AddWeapon(ABaseGun* AGun);

	ABaseGun* GetCurrentWeapon() { return Weapons[CurrentWeaponIndex]; }
private:
	void SwitchWeapon(int newIndex);
	bool WeaponExists(int index) { return Weapons[index] != nullptr; }
private:
	TArray<ABaseGun*> Weapons;
	int CurrentWeaponIndex = 0;
	
};
