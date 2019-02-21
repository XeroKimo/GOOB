// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUpActor.h"
#include "Components/SphereComponent.h"

#include "PlayerCharacter.h"
// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereCollision->SetSphereRadius(64.f);
    RootComponent = SphereCollision;
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SphereCollision->SetCollisionProfileName("OverlapAll");
    SphereCollision->SetGenerateOverlapEvents(true);

    SphereCollision->SetSimulatePhysics(false);

   // SphereCollision->OverlapComponent.Dynamic(this, &APickUpActor::OnOverlapBegin);

    Tags.Add("Pickup");
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

