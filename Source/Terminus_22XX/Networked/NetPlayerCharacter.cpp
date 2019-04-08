// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TimerManager.h"
#include "Networked/NetBaseGun.h"
#include "NetInventoryComponent.h"
#include "Terminus_22XXGameModeBase.h"
#include "Terminus_22XX_GameState.h"
#include "Networked/NetPlayerState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetPlayerCharacter::ANetPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    OnTakeAnyDamage.AddDynamic(this, &ANetPlayerCharacter::TakeAnyDamage);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->SetIsReplicated(true);


	GunLocation = CreateDefaultSubobject<USceneComponent>("Gun Location");
	GunLocation->SetupAttachment(FirstPersonCamera);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

	WeaponInventory = CreateDefaultSubobject<UNetInventoryComponent>("Inventory");

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioCopmonent");
	AudioComponent->SetupAttachment(RootComponent);

	JumpMaxCount = 2;
	//MaxHealth = 100.0f;
	//CurrentHealth = MaxHealth;
	//CurrentWeapon = nullptr;

	CurrentStatus = PreviousStatus = MovementStatus_Idle;

	SetReplicates(true);
	SetReplicateMovement(true);

	Tags.Add("Player");
}

// Called when the game starts or when spawned
void ANetPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	ServerAttachNewWeapon( WeaponInventory->GetAWeapon());
	//ServerSpawnGun();
}

void ANetPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents(); 
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	MaxPowerSuperJumpTime = FMath::Min(MaxPowerSuperJumpTime, ReleaseSuperJumpTime + SuperJumpTimerDelay);

	characterMovement->MaxWalkSpeed = MaxWalkSpeed;
	characterMovement->MaxAcceleration = MaxAcceleration;
	characterMovement->GravityScale = BaseGravityScale;
	characterMovement->JumpZVelocity = JumpZVelocity;
}

// Called every frame
void ANetPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    GEngine->AddOnScreenDebugMessage(-1, DeltaTime * 1.01f, FColor::Red, "See T_22XXGameModeBase in order to spawn at camera Location");


	if (GetVelocity().Size() == 0.0f)
		CurrentStatus = MovementStatus_Idle;
	else if (GetVelocity().Z != 0.0f)
		CurrentStatus = MovementStatus_Jumping;
	else
		CurrentStatus = MovementStatus_Walking;
	if (CurrentStatus == MovementStatus_Walking && (PreviousStatus & (CurrentStatus | MovementStatus_Idle)) && GetVelocity().Size() != 0.0f)
	{
		if (!AudioComponent->IsPlaying())
		{
			if (WalkingSound)
			{
				AudioComponent->Sound = WalkingSound;
				AudioComponent->Play();
			}
		}
	}
	else if (CurrentStatus == MovementStatus_Jumping && (PreviousStatus & (MovementStatus_Walking | MovementStatus_Idle)))
	{
		if (JumpingSound)
		{
			AudioComponent->Sound = JumpingSound;
			AudioComponent->Play();
		}
	}
	else if ((CurrentStatus & (MovementStatus_Walking | MovementStatus_Idle)) && PreviousStatus == MovementStatus_Jumping)
	{
		if (LandingSound)
		{
			AudioComponent->Sound = LandingSound;
			AudioComponent->Play();
		}
	}
	PreviousStatus = CurrentStatus;
}

// Called to bind functionality to input
void ANetPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	/*PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSideways", this, &APawn::AddControllerYawInput);*/

}

void ANetPlayerCharacter::MoveForeward(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val); 
	}
}

void ANetPlayerCharacter::MoveSideways(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ANetPlayerCharacter::LookUp(float Val)
{
	AddControllerPitchInput(BaseTurnSpeed*Val);
		if (Role < ROLE_Authority && GetController())
			ServerCameraRotate(GetController()->GetControlRotation());
}

void ANetPlayerCharacter::LookSideways(float Val)
{
	AddControllerYawInput(BaseTurnSpeed*Val);
}

void ANetPlayerCharacter::Jump()
{
	/*if (Role == ROLE_AutonomousProxy)
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Jump Autonomous");
	if (Role == ROLE_Authority)
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "Jump Authority");*/

	if (!GetController()->IsLocalController())
		return;
	if (!CanJump())
		return;

	if (GetCharacterMovement()->IsFalling() || JumpCurrentCount >= 1)
		PrepareSuperJump();
	else
	{
		Super::Jump();
	}
}

void ANetPlayerCharacter::StopJumping()
{
	if (GetController()->IsLocalController())
	{
		FTimerManager& timerManager = GetWorldTimerManager();

		if (GetWorldTimerManager().TimerExists(SuperJumpTimer))
		{
			ReleaseSuperJump();
		}
		GetWorldTimerManager().ClearTimer(SlowDescentTimer);
		GetWorldTimerManager().ClearTimer(SuperJumpTimer);
		ServerNormalDescent();
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Normal");
		Super::StopJumping();
	}
}

void ANetPlayerCharacter::PrepareSuperJump()
{
	GetWorldTimerManager().SetTimer(SuperJumpTimer, this, &ANetPlayerCharacter::StopJumping, ReleaseSuperJumpTime, false, ReleaseSuperJumpTime + SuperJumpTimerDelay);
	GetWorldTimerManager().SetTimer(SlowDescentTimer, this, &ANetPlayerCharacter::ServerForceStopAndSlowDescent, SuperJumpTimerDelay, false);
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Player Jumped");

	if (ChargingSound)
	{
		AudioComponent->Sound = ChargingSound;
		AudioComponent->Play();
	}
	if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
		JumpCurrentCount++;

	JumpCurrentCount++;

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Jump Current Count: " + FString::FromInt(JumpCurrentCount));
}

void ANetPlayerCharacter::ReleaseSuperJump()
{
	if (GetWorldTimerManager().IsTimerActive(SlowDescentTimer) == false)
	{
		float totalTimeForSJTimer = ReleaseSuperJumpTime + SuperJumpTimerDelay;
		float elapsedTime = GetWorldTimerManager().GetTimerElapsed(SuperJumpTimer);
		if (elapsedTime >= MaxPowerSuperJumpTime + SuperJumpTimerDelay)
			elapsedTime = 1.0f;

		float MaxSuperJump = JumpZVelocity * MaxSuperJumpPowerScale;

		FVector cameraDirection = FirstPersonCamera->GetComponentRotation().Vector().GetSafeNormal();
		FVector baseForce = cameraDirection * JumpZVelocity;
		FVector storedForce = cameraDirection * StoredSpeedBeforeJump * (BaseSuperJumpMultiplier + AddedSuperJumpMultiplier * (elapsedTime / 1.0f));
		FVector newForceVector = (baseForce + storedForce);

		if (newForceVector.Size() > MaxSuperJump)
		{
			newForceVector = newForceVector.GetUnsafeNormal() * MaxSuperJump * 0.999997f;
		}

		if (ReleaseSuperJumpSound)
		{
			AudioComponent->Sound = ReleaseSuperJumpSound;
			AudioComponent->Play();
		}
		else
			AudioComponent->Stop();

		ServerSuperJump(newForceVector);
	}
	else
	{
		float cancelVector = GetVelocity().Z;
		FVector jumpVector = GetActorUpVector() * (GetCharacterMovement()->JumpZVelocity - cancelVector);
		ServerSuperJump(jumpVector);
	}
}

void ANetPlayerCharacter::FireWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->PullTrigger();
	FRotator camRot = FirstPersonCamera->GetComponentRotation();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Gun Rotation - " + FString::SanitizeFloat(camRot.Yaw) + " , " + FString::SanitizeFloat(camRot.Pitch));
}

void ANetPlayerCharacter::StopFireWeapon()
{
	if (!GetController()->IsLocalController())
		return;
	if (!CurrentWeapon)
		return;
	CurrentWeapon->ReleaseTrigger();
}

void ANetPlayerCharacter::ServerCameraRotate_Implementation(FRotator rot)
{
	//FRotator camRot = FirstPersonCamera->GetComponentRotation();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Gun Rotation - " + FString::SanitizeFloat(camRot.Yaw) + " , " + FString::SanitizeFloat(camRot.Pitch));
	FirstPersonCamera->SetWorldRotation(rot);
}

bool ANetPlayerCharacter::ServerCameraRotate_Validate(FRotator rot)
{
	return true;
}

void ANetPlayerCharacter::NormalDescent()
{
	GetCharacterMovement()->GravityScale = BaseGravityScale;
}

void ANetPlayerCharacter::ServerAttachNewWeapon_Implementation(ANetBaseGun * nextGun)
{
    if (nextGun != nullptr && CurrentWeapon != nullptr)
    {
        if (nextGun->GetWeaponIndex() != CurrentWeapon->GetWeaponIndex())
        {
            CurrentWeapon->FullStop();
            CurrentWeapon->ServerDetach();
            CurrentWeapon = nextGun;
            CurrentWeapon->ServerAttach(this);
        }
    }
    else if (CurrentWeapon == nullptr)
    {
        CurrentWeapon = nextGun;
        if (CurrentWeapon)
            CurrentWeapon->ServerAttach(this);
    }
}
bool ANetPlayerCharacter::ServerAttachNewWeapon_Validate(ANetBaseGun * nextGun)
{
    return true;
}

void ANetPlayerCharacter::NetMulticastNormalDescent_Implementation()
{
	NormalDescent();
}

bool ANetPlayerCharacter::NetMulticastNormalDescent_Validate()
{
	return true;
}
void ANetPlayerCharacter::NetMulticastForceStopAndSlowDescent_Implementation()
{
	if (Role == ROLE_Authority)
		StoreCurrentSpeed();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = WeakenedGravityScale;
}

bool ANetPlayerCharacter::NetMulticastForceStopAndSlowDescent_Validate()
{
	return true;
}
void ANetPlayerCharacter::ServerSuperJump_Implementation(FVector Impulse)
{
	GetCharacterMovement()->AddImpulse(Impulse, true);
}

void ANetPlayerCharacter::ServerNormalDescent_Implementation()
{
	NetMulticastNormalDescent();
}

bool ANetPlayerCharacter::ServerNormalDescent_Validate()
{
	return true;
}
bool ANetPlayerCharacter::ServerSuperJump_Validate(FVector Impulse)
{
	if (Impulse.Size() > JumpZVelocity * MaxSuperJumpPowerScale)
		return false;
	else
		return true;
}

void ANetPlayerCharacter::ServerForceStopAndSlowDescent_Implementation()
{
	NetMulticastForceStopAndSlowDescent();
	//StoreCurrentSpeed();
	//GetCharacterMovement()->StopActiveMovement();
	//GetCharacterMovement()->StopMovementImmediately();
	//GetCharacterMovement()->GravityScale = WeakenedGravityScale;
}

bool ANetPlayerCharacter::ServerForceStopAndSlowDescent_Validate()
{
	return true;
}

void ANetPlayerCharacter::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
    GetPlayerState()->CurrentHealth -= Damage;
    if (GetPlayerState()->CurrentHealth <= 0)
    {
        Respawn();
    }
}

void ANetPlayerCharacter::AddScore(int score)
{
	GetPlayerState()->PlayerScore += score;
}

void ANetPlayerCharacter::StoreCurrentSpeed()
{
	StoredSpeedBeforeJump = GetVelocity().Size();
}

bool ANetPlayerCharacter::AddWeaponToInventory(ANetBaseGun * AGun)
{
	if (WeaponInventory->AddWeapon(AGun))
	{
		AGun->SetOwner(this);
		ServerAttachNewWeapon(WeaponInventory->SwitchToGun(AGun->GetWeaponIndex()));
		GetPlayerState()->CurrentGuns.Add(AGun);
		return true;
	}
	return false;
}

void ANetPlayerCharacter::Reload()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->PrepareReload();
    }
}

void ANetPlayerCharacter::SwitchWeapon(float Val)
{
    if (EnableWeaponScrolling)
    {
        if (Val > 0)
            NextWeapon();
        else if (Val < 0)
            PreviousWeapon();
    }
}

void ANetPlayerCharacter::NextWeapon()
{
    if (!GetWorldTimerManager().IsTimerActive(WeaponSwitchTimer))
    {
        ServerAttachNewWeapon(WeaponInventory->NextWeapon());
        GetWorldTimerManager().SetTimer(WeaponSwitchTimer, WeaponSwitchLockout, false);
    }
}

void ANetPlayerCharacter::PreviousWeapon()
{
    if (!GetWorldTimerManager().IsTimerActive(WeaponSwitchTimer))
    {
        ServerAttachNewWeapon(WeaponInventory->PreviousWeapon());
        GetWorldTimerManager().SetTimer(WeaponSwitchTimer, WeaponSwitchLockout, false);
    }
}

void ANetPlayerCharacter::SwitchToShotgun()
{
    ServerAttachNewWeapon(WeaponInventory->GetShotgun());
}

void ANetPlayerCharacter::SwitchToVampyr()
{
    ServerAttachNewWeapon(WeaponInventory->GetVampyr());
}

void ANetPlayerCharacter::SwitchToRailgun()
{
    ServerAttachNewWeapon(WeaponInventory->GetRailgun());
}

void ANetPlayerCharacter::LogIn()
{
    if (Role == ROLE_Authority)
        GetGameState()->ConnectedPlayers.Add(PlayerState);
}

void ANetPlayerCharacter::Respawn()
{
    if (Role == ROLE_Authority)
    {
        ATerminus_22XXGameModeBase* mode = Cast<ATerminus_22XXGameModeBase>(GetGameState()->AuthorityGameMode);
        if (mode)
        {
            mode->RespawnPlayer(Cast<APlayerController>(GetController()));
        }
    }
    if (Role < ROLE_Authority)
    {
        ServerRespawn();
    }
}

void ANetPlayerCharacter::SetPlayerState(APlayerState * state)
{
    PlayerState = state;
    TArray<ANetBaseGun*> guns = GetPlayerState()->CurrentGuns;
    for (int i = 0; i < guns.Num(); i++)
    {
        AddWeaponToInventory(guns[i]);
    }
    GetPlayerState()->CurrentHealth = MaxHealth;
	GetPlayerState()->MaxHealth = MaxHealth;
}

void ANetPlayerCharacter::SetActiveCheckpoint(int CheckpointID)
{
	if (GetPlayerState())
	{
		GetPlayerState()->CurrentCheckpointID = CheckpointID;
	}
}

ANetPlayerState * ANetPlayerCharacter::GetPlayerState()
{
    return  Cast<ANetPlayerState>(PlayerState);
}

ATerminus_22XX_GameState* ANetPlayerCharacter::GetGameState()
{
    return Cast<ATerminus_22XX_GameState>(GetWorld()->GetGameState());
}

void ANetPlayerCharacter::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //DOREPLIFETIME_CONDITION(ANetPlayerCharacter, SuperJumpTimer, COND_OwnerOnly);
    //DOREPLIFETIME_CONDITION(ANetPlayerCharacter, SlowDescentTimer, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ANetPlayerCharacter, StoredSpeedBeforeJump, COND_OwnerOnly);
    DOREPLIFETIME(ANetPlayerCharacter, CurrentWeapon);

}

void ANetPlayerCharacter::ServerRespawn_Implementation()
{
    Respawn();
}
bool ANetPlayerCharacter::ServerRespawn_Validate()
{
    return true;
}