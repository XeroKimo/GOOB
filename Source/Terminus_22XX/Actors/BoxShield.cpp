// Fill out your copyright notice in the Description page of Project Settings.

#include "BoxShield.h"
#include "Components/BoxShieldComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABoxShield::ABoxShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Shield = CreateDefaultSubobject<UBoxShieldComponent>("Shield");
	RootComponent = Shield->GetBox();

}

// Called when the game starts or when spawned
void ABoxShield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

