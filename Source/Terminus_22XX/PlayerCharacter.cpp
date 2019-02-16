// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TimerManager.h"
#include "BaseGun.h"
#include "T22XX_Shotgun.h"

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

	GunLocation = CreateDefaultSubobject<USceneComponent>("Gun Location");
	GunLocation->SetupAttachment(CharacterMesh);


	JumpMaxCount = 2;
	bUseControllerRotationPitch = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay(); 
	

	
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (DebugWeapon != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ABaseGun* weapon = GetWorld()->SpawnActor<ABaseGun>(DebugWeapon, SpawnParams);
		CurrentWeapon = weapon;
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->Attach(this);
		CurrentWeapon->SetActorRelativeLocation(GunLocation->RelativeLocation);
	}

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	MaxPowerSuperJumpTime = FMath::Min(MaxPowerSuperJumpTime, ReleaseSuperJumpTime + SuperJumpTimerDelay);

	characterMovement->MaxWalkSpeed = MaxWalkSpeed;
	characterMovement->MaxAcceleration = MaxAcceleration;
	characterMovement->GravityScale = BaseGravityScale;
	characterMovement->JumpZVelocity = JumpZVelocity;

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
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &APlayerCharacter::StopFireWeapon);
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
	bool isFalling = GetCharacterMovement()->IsFalling();
	if ((isFalling || JumpCurrentCount >= 1) && CanJump())
	{
		GetWorldTimerManager().SetTimer(SuperJumpTimer, this, &APlayerCharacter::StopJumping, ReleaseSuperJumpTime, false, ReleaseSuperJumpTime + SuperJumpTimerDelay );
		GetWorldTimerManager().SetTimer(SlowDescentTimer, this, &APlayerCharacter::ForceStopAndSlowDescent, SuperJumpTimerDelay, false);

		if (JumpCurrentCount == 0 && isFalling)
			JumpCurrentCount++;

		JumpCurrentCount++;
	}
	else 
		Super::Jump();

}

void APlayerCharacter::StopJumping()
{
	FTimerManager& timerManager = GetWorldTimerManager();

	if (timerManager.TimerExists(SuperJumpTimer))
	{
		if (timerManager.IsTimerActive(SlowDescentTimer) == false)
		{
			float totalTimeForSJTimer = ReleaseSuperJumpTime + SuperJumpTimerDelay;
			float elapsedTime = timerManager.GetTimerElapsed(SuperJumpTimer);
			if (elapsedTime >= MaxPowerSuperJumpTime + SuperJumpTimerDelay)
				elapsedTime = 1.0f;

			float MaxSuperJump = JumpZVelocity * MaxSuperJumpPowerScale;

			FVector cameraDirection = FirstPersonCamera->GetComponentRotation().Vector().GetSafeNormal();
			FVector baseForce = cameraDirection * JumpZVelocity;
			FVector storedForce = cameraDirection * StoredSpeedBeforeJump * (BaseSuperJumpMultiplier + AddedSuperJumpMultiplier * (elapsedTime / 1.0f));
			FVector newForceVector = (baseForce + storedForce);

			if (newForceVector.Size() > MaxSuperJump)
			{
				newForceVector = newForceVector.GetUnsafeNormal() * MaxSuperJump;
			}

			
			GetCharacterMovement()->AddImpulse(newForceVector, true);
		}
		else
		{
            float cancelVector = GetVelocity().Z;
			FVector jumpVector = GetActorUpVector() * (GetCharacterMovement()->JumpZVelocity - cancelVector);
			GetCharacterMovement()->AddImpulse(jumpVector, true);
		}
		GetWorldTimerManager().ClearTimer(SlowDescentTimer);
		GetWorldTimerManager().ClearTimer(SuperJumpTimer);
	}
	GetCharacterMovement()->GravityScale = BaseGravityScale;
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Normal");
	Super::StopJumping();
}



void APlayerCharacter::FireWeapon()
{
	if (CurrentWeapon)
	{
        //GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Character Rotation: " + FString::SanitizeFloat(GetControlRotation().Yaw) + " : "+ FString::SanitizeFloat(GetControlRotation().Pitch));
		CurrentWeapon->PullTrigger();
	}
}

void APlayerCharacter::StopFireWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ReleaseTrigger();
	}
}

void APlayerCharacter::ForceStopAndSlowDescent()
{
	StoreCurrentSpeed();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = WeakenedGravityScale;
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Lowered");
}

void APlayerCharacter::StoreCurrentSpeed()
{
	StoredSpeedBeforeJump = GetVelocity().Size();
}