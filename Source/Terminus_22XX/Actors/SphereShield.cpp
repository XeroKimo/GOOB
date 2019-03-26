// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereShield.h"
#include "Components/SphereShieldComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASphereShield::ASphereShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Shield = CreateDefaultSubobject<USphereShieldComponent>("Shield");
	RootComponent = Shield;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASphereShield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASphereShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

