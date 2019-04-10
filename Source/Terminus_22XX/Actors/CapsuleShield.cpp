// Fill out your copyright notice in the Description page of Project Settings.

#include "CapsuleShield.h"
#include "Components/CapsuleShieldComponent.h"
#include "Components/CapsuleComponent.h"


//A actor with a capsule shield
// Sets default values
ACapsuleShield::ACapsuleShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Shield = CreateDefaultSubobject<UCapsuleShieldComponent>("Shield");
	RootComponent = Shield;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void ACapsuleShield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACapsuleShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

