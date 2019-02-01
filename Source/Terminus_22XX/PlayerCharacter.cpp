// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TimerManager.h"

#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	CharacterMesh->SetupAttachment(RootComponent);
	CharacterMesh->SetCollisionProfileName("NoCollision");

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForeward", this, &APlayerCharacter::MoveForeward);
	PlayerInputComponent->BindAxis("MoveSideways", this, &APlayerCharacter::MoveSideways);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSideways", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &APlayerCharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &APlayerCharacter::FireWeapon);
}

void APlayerCharacter::MoveForeward(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void APlayerCharacter::MoveSideways(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void APlayerCharacter::LookUp(float Val)
{
	AddControllerPitchInput(BaseTurnSpeed*Val*GetWorld()->DeltaTimeSeconds);
}

void APlayerCharacter::LookSideways(float Val)
{
	AddControllerYawInput(BaseTurnSpeed*Val*GetWorld()->DeltaTimeSeconds);
}

void APlayerCharacter::Jump()
{
	if (JumpCurrentCount == 0)
		Super::Jump();
	else if (CanJump())
	{
		GetWorldTimerManager().SetTimer(SuperJumpTimer, this, &APlayerCharacter::StopJumping, 10.f, false, ReleaseSuperJumpTime + SuperJumpTimerDelay);
		GetWorldTimerManager().SetTimer(SlowDescentTimer, this, &APlayerCharacter::ForceStopAndSlowDescent, 10.f, false, SuperJumpTimerDelay);
		JumpCurrentCount++;
	}
	//AddMovementInput(GetActorUpVector(), JumpForce,true);

}

void APlayerCharacter::StopJumping()
{
	if (JumpCurrentCount == 2 && !CanJump())
	{
		if (GetWorldTimerManager().IsTimerActive(SlowDescentTimer) == false)
		{
			float elapsedTime = GetWorldTimerManager().GetTimerRemaining(SuperJumpTimer);
			if (elapsedTime > 1.0f + SuperJumpTimerDelay)
				elapsedTime = 1.0f;
			FVector cameraDirection = FirstPersonCamera->GetComponentRotation().Vector().GetSafeNormal();
			FVector baseForce = cameraDirection * GetCharacterMovement()->JumpZVelocity;
			FVector storedForce = cameraDirection * StoredSpeedBeforeJump * (BaseSuperJumpMultiplier + AddedSuperJumpMultiplier * (elapsedTime / 1.0f));
			FVector newForceVector = (baseForce + storedForce) * JumpForce;
			GetCharacterMovement()->AddForce(newForceVector);
		}
		else
		{
			FVector jumpVector = GetActorUpVector() * GetCharacterMovement()->JumpZVelocity*JumpForce;
			GetCharacterMovement()->AddForce(jumpVector);
		}
		GetWorldTimerManager().ClearTimer(SlowDescentTimer);
		GetWorldTimerManager().ClearTimer(SuperJumpTimer);

		JumpCurrentCount++;
	}
	GetCharacterMovement()->GravityScale = 1.0f;
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Normal");
	Super::StopJumping();
}



void APlayerCharacter::FireWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Weapon Fired");
}

void APlayerCharacter::ForceStopAndSlowDescent()
{
	StoreCurrentSpeed();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = 0.05f;
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Lowered");
}

void APlayerCharacter::StoreCurrentSpeed()
{
	StoredSpeedBeforeJump = GetVelocity().Size();
}