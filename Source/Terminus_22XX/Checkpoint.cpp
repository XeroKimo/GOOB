// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Networked/NetPlayerCharacter.h"

ACheckpoint::ACheckpoint(const FObjectInitializer & ObjectInitializer) :
    APlayerStart(ObjectInitializer)
{
	PlayerDetection = CreateDefaultSubobject<UBoxComponent>("Player Detector");
	PlayerDetection->SetCollisionProfileName("OverlapOnlyPawn");
    PlayerDetection->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	PlayerDetection->SetupAttachment(RootComponent);
	PlayerDetection->SetGenerateOverlapEvents(true);
	PlayerDetection->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ANetPlayerCharacter* player = Cast<ANetPlayerCharacter>(OtherActor))
	{
		if (Role == ROLE_Authority)
			player->SetActiveCheckpoint(CheckpointID);
	}
}