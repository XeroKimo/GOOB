// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Checkpoint.generated.h"

/**
 * 
 */
UCLASS()
class TERMINUS_22XX_API ACheckpoint : public APlayerStart
{
    GENERATED_BODY()
public:
    ACheckpoint(const FObjectInitializer & ObjectInitializer);
	
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	//Players will spawn at a checkpoint with a corresponding ID
    UPROPERTY(EditInstanceOnly, Category = "ID")
        int CheckpointID = 0;
	//Indicate if this is the last checkpoint
	//To start the boss countdown
	UPROPERTY(EditInstanceOnly, Category = "ID")
		bool FinalCheckpoint;

protected:
	UPROPERTY(VisibleAnywhere, Category = "HitDetection")
		class UBoxComponent* PlayerDetection;
};
