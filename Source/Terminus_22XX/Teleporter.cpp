// Fill out your copyright notice in the Description page of Project Settings.

#include "Teleporter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Public/TimerManager.h"

// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    Hitbox = CreateDefaultSubobject<UBoxComponent>("Hitbox");
    Hitbox->SetCollisionProfileName("OverlapOnlyPawn");
    Hitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    Hitbox->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnOverlapBegin);
    Hitbox->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnOverlapEnd);
    RootComponent = Hitbox;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetCollisionProfileName("NoCollision");
    Mesh->SetupAttachment(RootComponent);

    PortingActor = nullptr;
}

// Called when the game starts or when spawned
void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	PortingActor = nullptr;
}

// Called every frame
void ATeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleporter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Make sure the overlapped actor is a player
    if (OtherActor->ActorHasTag("Player"))
    {
		//Set the player to be the actor that will be teleported
        PortingActor = OtherActor;
		//Start the teleport timer
        GetWorldTimerManager().SetTimer(TeleportTimer, this, &ATeleporter::ServerMoveActor, TeleporterDelay, false);
    }
}

void ATeleporter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor == PortingActor)
    {
		//If the player leaves the teleporting box
		//Stop the timer
        GetWorldTimerManager().ClearTimer(TeleportTimer);
		//Reset the porting actor
        PortingActor = nullptr;
    }
}

void ATeleporter::ServerMoveActor_Implementation()
{
	//Teleport the actor to the destination point location
    PortingActor->SetActorLocation(DestinationPoint->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
    PortingActor = nullptr;
}
bool ATeleporter::ServerMoveActor_Validate()
{
    return true;
}


