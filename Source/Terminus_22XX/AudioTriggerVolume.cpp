// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioTriggerVolume.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAudioTriggerVolume::AAudioTriggerVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Trigger Volume");
	RootComponent = TriggerVolume;
	TriggerVolume->SetCollisionProfileName("Trigger");
	TriggerVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AAudioTriggerVolume::OnOverlapBegin);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->SetupAttachment(RootComponent);
	//AudioComponent->AttenuationSettings->Attenuation.bAttenuate = false;

}

// Called when the game starts or when spawned
void AAudioTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAudioTriggerVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAudioTriggerVolume::NetMulticastPlayAudio_Implementation()
{
	PlayAudio();
}

void AAudioTriggerVolume::PlayAudio()
{
	if (!SoundClip)
		return;

	if (AudioComponent->IsPlaying())
		return;

	AudioComponent->Sound = SoundClip;
	AudioComponent->Play();
}

void AAudioTriggerVolume::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor->ActorHasTag("Player"))
		return;
	if (HasBeenTriggered && TriggerOnce)
		return;

	PlayAudio();
	if (Role == ROLE_Authority)
	{
		if (GlobalTrigger)
			NetMulticastPlayAudio();
		HasBeenTriggered = true;
	}
}

void AAudioTriggerVolume::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAudioTriggerVolume, HasBeenTriggered);
}
