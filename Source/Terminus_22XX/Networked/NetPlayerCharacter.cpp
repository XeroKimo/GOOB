// Fill out your copyright notice in the Description page of Project Settings.

#include "NetPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TimerManager.h"
#include "Networked/NetBaseGun.h"
#include "NetInventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Terminus_22XXGameModeBase.h"
#include "Terminus_22XX_GameState.h"
#include "Networked/NetPlayerState.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetPlayerCharacter::ANetPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->SetIsReplicated(true);


	GunLocation = CreateDefaultSubobject<USceneComponent>("Gun Location");
	GunLocation->SetupAttachment(FirstPersonCamera);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	WeaponInventory = CreateDefaultSubobject<UNetInventoryComponent>("Inventory");

	JumpMaxCount = 2;
	//MaxHealth = 100.0f;
	//CurrentHealth = MaxHealth;
	//CurrentWeapon = nullptr;

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
	AddControllerPitchInput(BaseTurnSpeed*Val*GetWorld()->DeltaTimeSeconds);
		if (Role < ROLE_Authority)
			ServerCameraDebug(GetController()->GetControlRotation());
}

void ANetPlayerCharacter::LookSideways(float Val)
{
	AddControllerYawInput(BaseTurnSpeed*Val*GetWorld()->DeltaTimeSeconds);
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

void ANetPlayerCharacter::ServerSpawnGun_Implementation()
{
	if (testGun != NULL)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParams.Owner = this;
		spawnParams.Instigator = this;
		ANetBaseGun* spawnedGun = GetWorld()->SpawnActor<ANetBaseGun>(testGun,spawnParams);
		if (spawnedGun)
		{
			spawnedGun->ServerAttach(this);
			CurrentWeapon = spawnedGun;
		}

	}
}

bool ANetPlayerCharacter::ServerSpawnGun_Validate()
{
	return true;
}

void ANetPlayerCharacter::ServerCameraDebug_Implementation(FRotator rot)
{
	//FRotator camRot = FirstPersonCamera->GetComponentRotation();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Gun Rotation - " + FString::SanitizeFloat(camRot.Yaw) + " , " + FString::SanitizeFloat(camRot.Pitch));
	FirstPersonCamera->SetWorldRotation(rot);
}

bool ANetPlayerCharacter::ServerCameraDebug_Validate(FRotator rot)
{
	return true;
}

void ANetPlayerCharacter::NormalDescent()
{
	GetCharacterMovement()->GravityScale = BaseGravityScale;
}

void ANetPlayerCharacter::ServerResetPickupState_Implementation()
{
    PickupSuccess = false;
    OnRep_PickupSuccess();
}
bool ANetPlayerCharacter::ServerResetPickupState_Validate()
{
    return true;
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

void ANetPlayerCharacter::OnRep_PickupSuccess()
{
}

void ANetPlayerCharacter::StoreCurrentSpeed()
{
	StoredSpeedBeforeJump = GetVelocity().Size();
}
//
//bool ANetPlayerCharacter::AddWeaponToInventory_Implementation(ANetBaseGun* AGun)
//{
//    ServerAddWeaponToInvetory(AGun);
//    if (PickupSuccess)
//    {
//        ServerResetPickupState();
//        OnRep_PickupSuccess();
//        return true;
//    }
//    return false;
//}

void ANetPlayerCharacter::ServerAddWeaponToInvetory_Implementation(ANetBaseGun * AGun)
{
    if (WeaponInventory->AddWeapon(AGun))
    {
        AGun->SetOwner(this);
        ServerAttachNewWeapon(WeaponInventory->SwitchToGun(AGun->GetWeaponIndex()));
        PickupSuccess = true;
        OnRep_PickupSuccess();
    }
}

bool ANetPlayerCharacter::ServerAddWeaponToInvetory_Validate(ANetBaseGun * AGun)
{
    return true;
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
}

void ANetPlayerCharacter::SetPlayerState(APlayerState * state)
{
    PlayerState = state;
    TArray<ANetBaseGun*> guns = GetPlayerState()->CurrentGuns;
    for (int i = 0; i < guns.Num(); i++)
    {
        ServerAddWeaponToInvetory(guns[i]);
    }
    GetPlayerState()->CurrentHealth = MaxHealth;
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
    DOREPLIFETIME_CONDITION(ANetPlayerCharacter, PickupSuccess, COND_OwnerOnly);
    DOREPLIFETIME(ANetPlayerCharacter, CurrentWeapon);

}