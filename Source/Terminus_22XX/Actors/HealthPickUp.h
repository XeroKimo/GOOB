// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpActor.h"
#include "HealthPickUp.generated.h"

/**
 * 
 */


 // ---------- DEPRECATED CLASS ---------- //
UCLASS()
class TERMINUS_22XX_API AHealthPickUp : public APickUpActor
{
    GENERATED_BODY()

protected:

        UPROPERTY(Category = "Health", EditAnywhere)
        class UStaticMeshComponent* HealthMesh;

		UPROPERTY(Category = "Health", EditAnywhere)
			float HealAmount;
	
public:
    AHealthPickUp();
   
protected:
        virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
          
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);
};
