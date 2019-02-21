// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"



// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Tags.Empty();
    Tags.Add("AIGuard");

    //Create Enemy
    EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    EnemyMesh->SetCollisionProfileName("No Collision");
    RootComponent = EnemyMesh;

    //Create Hitbox
    HitBox = CreateDefaultSubobject<UCapsuleComponent>("HitBox");
    HitBox->SetCollisionProfileName("BlockAll");
    HitBox->SetNotifyRigidBodyCollision(true);
    HitBox->SetupAttachment(RootComponent);

    bCanBeDamage = true;

    //Enemy Take Damage
    OnTakeAnyDamage.AddDynamic(this, &AEnemy::TakeAnyDamage);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    if (bPatrol)
    {
        MoveToNextPatrolPoint();
    }
    
	
}

void AEnemy::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Damage Received - " + FString::FromInt(Damage));
    EnemyHealth = -Damage;
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, "Health - " + FString::FromInt(EnemyHealth));
}

void AEnemy::MoveToNextPatrolPoint()
{
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "MoveToNextPatrolPoint - ");
    // Assign next patrol point.

    if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
    {
        CurrentPatrolPoint = FirstPatrolPoint;

    }
    else
    {
        CurrentPatrolPoint = SecondPatrolPoint;
    }

    FVector Direction = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
    Direction.Normalize();

    FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
    NewLookAt.Pitch = 0.0f;
    NewLookAt.Roll = 0.0f;
    NewLookAt.Yaw += -180.0f;
    SetActorRotation(NewLookAt);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Patrol Goal Checks
    if (CurrentPatrolPoint)
    {
        FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
        float DistanceToGoal = Delta.Size();

        // Check if we are within 50 .. 100(Better with 2D) units of our goal, if so - pick a new patrol point
        if (DistanceToGoal < 100)
        {
            MoveToNextPatrolPoint();
        }
        SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), CurrentPatrolPoint->GetActorLocation(), DeltaTime, 600.f));
    }

}

