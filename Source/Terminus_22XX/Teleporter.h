// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

UCLASS()
class TERMINUS_22XX_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION(Server, Reliable, WithValidation)
        void ServerMoveActor();

protected:
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* Mesh;
    UPROPERTY(VisibleAnywhere)
        class UBoxComponent* Hitbox;

    UPROPERTY(EditAnywhere, Category = "Config")
        float TeleporterDelay = 1.5f;
    UPROPERTY(EditAnywhere, Category = "Config")
        AActor* DestinationPoint;

    FTimerHandle TeleportTimer;

    AActor* PortingActor;
	
};
