// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioTriggerVolume.generated.h"

UCLASS()
class TERMINUS_22XX_API AAudioTriggerVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioTriggerVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Config")
		class USoundBase* SoundClip;
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		bool TriggerOnce = true;

	UPROPERTY(EditAnywhere, Category = "Config")
		bool GlobalTrigger = false;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* TriggerVolume;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* AudioComponent;

private:
	UPROPERTY(Replicated)
	bool HasBeenTriggered = false;
private:
	void PlayAudio();

	UFUNCTION(NetMulticast, Reliable)
		void NetMulticastPlayAudio();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	


};
