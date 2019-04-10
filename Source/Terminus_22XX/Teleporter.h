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

	//Teleports the actor
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerMoveActor();

protected:
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* Mesh;
    UPROPERTY(VisibleAnywhere)
        class UBoxComponent* Hitbox;

	//How long a actor must overlap the hitbox before teleporting
    UPROPERTY(EditAnywhere, Category = "Config")
        float TeleporterDelay = 1.5f;
	//The point which the porting actor will teleport to
    UPROPERTY(EditAnywhere, Category = "Config")
        AActor* DestinationPoint;

    FTimerHandle TeleportTimer;

	//The actor that will get teleported
    AActor* PortingActor;
	
};
