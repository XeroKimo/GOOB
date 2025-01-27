// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

UCLASS()
class TERMINUS_22XX_API APickUpActor : public AActor
{
    GENERATED_BODY()

protected:
        UPROPERTY(Category = "PickUp", EditAnywhere)
         class USphereComponent* SphereCollision;
	
public:	
	// Sets default values for this actor's properties
	APickUpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

 
	
};
