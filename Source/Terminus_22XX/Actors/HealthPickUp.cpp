// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickUp.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"


AHealthPickUp::AHealthPickUp() 
{

    PrimaryActorTick.bCanEverTick = true;

    HealthMesh = CreateDefaultSubobject<UStaticMeshComponent>("Health");
   // HealthMesh->SetCollisionProfileName("Block All");
    HealthMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HealthMesh->SetupAttachment(RootComponent);

    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickUp::OnOverlapBegin);
}

void AHealthPickUp::BeginPlay()
{
    Super::BeginPlay();
}

void AHealthPickUp::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AHealthPickUp::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (OtherActor)
    {
        APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
        if (Player)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
                "AMyPickupActor::OnOverlapBegin Overlapped with - "
                + OtherActor->GetName());

            Destroy();
        }

    }
}