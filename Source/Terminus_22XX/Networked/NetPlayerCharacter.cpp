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
	//Debug function
	//ServerSpawnGuns();
    //Get the first weapon in the inventory and attach to player
	ServerAttachNewWeapon( WeaponInventory->GetAWeapon());
}

void ANetPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents(); 
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	MaxPowerSuperJumpTime = FMath::Min(MaxPowerSuperJumpTime + SuperJumpTimerDelay, ReleaseSuperJumpTime + SuperJumpTimerDelay);

    //Change the character movement's default values to ones we set
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

    //Determine what kind of movement our player is doing
	if (GetVelocity().Size() == 0.0f)
		CurrentStatus = MovementStatus_Idle;
	else if (GetVelocity().Z != 0.0f)
		CurrentStatus = MovementStatus_Jumping;
	else
		CurrentStatus = MovementStatus_Walking;

    //Play a walking sound if the character is walking
    if (CurrentStatus == MovementStatus_Walking && (PreviousStatus & (CurrentStatus | MovementStatus_Idle)) && GetVelocity().Size() != 0.0f)
    {
        if (!AudioComponent->IsPlaying())
        {
            if (WalkingSound)
            {
                ServerPlaySound(WalkingSound);
            }
        }
    }
    //Stop walking sound if the player stops moving mid walk
    else if (CurrentStatus == MovementStatus_Idle && PreviousStatus == MovementStatus_Walking)
        ServerPlaySound(nullptr);
    //Play a jumping sound when the character jumps
	else if (CurrentStatus == MovementStatus_Jumping && (PreviousStatus & (MovementStatus_Walking | MovementStatus_Idle)))
	{
		if (JumpingSound)
		{
            ServerPlaySound(JumpingSound);
		}
	}
    //Play a landing sound when the character lands
	else if ((CurrentStatus & (MovementStatus_Walking | MovementStatus_Idle)) && PreviousStatus == MovementStatus_Jumping)
	{
		if (LandingSound)
		{
            ServerPlaySound(LandingSound);
		}
	}
    //Keep track of the previous status to properly get the right sound
	PreviousStatus = CurrentStatus;
}

// Called to bind functionality to input
void ANetPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANetPlayerCharacter::MoveForeward(float Val)
{
    //Moves character backwards or forewards
	if (Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val); 
	}
}

void ANetPlayerCharacter::MoveSideways(float Val)
{
    //Moves character sideways
	if (Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ANetPlayerCharacter::LookUp(float Val)
{
	AddControllerPitchInput(BaseTurnSpeed*Val);
    //Update the camera pitch on the server to match the client rotation
		if (Role < ROLE_Authority && GetController())
            ServerCameraUpdateRotation(GetController()->GetControlRotation());
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

    //Check to see if it's a local player is controlling the character
	if (!GetController()->IsLocalController())
		return;
    //Check to see if the character can jump
	if (!CanJump())
		return;

    //If the character is already in the air, or is on it's second jump
    //Prepare a super jump
    if (GetCharacterMovement()->IsFalling() || JumpCurrentCount >= 1)
        PrepareSuperJump();
	else
	{
        //Do a default jump
		Super::Jump();
	}
}

void ANetPlayerCharacter::StopJumping()
{
    //Check to see if it's a client controlling the character
    if (GetController())
    {
        if (GetController()->IsLocalController())
        {
            FTimerManager& timerManager = GetWorldTimerManager();

            //Check to see if a super jump is being prepared
            if (GetWorldTimerManager().TimerExists(SuperJumpTimer))
            {
                ReleaseSuperJump();
            }
            //Reset the slow descending timer
            GetWorldTimerManager().ClearTimer(SlowDescentTimer);
            //Reset the super jump timer
            GetWorldTimerManager().ClearTimer(SuperJumpTimer);
            //Return the player's gravity to normal
            ServerNormalDescent();
            //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Gravity Scale Normal");
            //Stop jumping
            Super::StopJumping();
        }
    }
}

void ANetPlayerCharacter::PrepareSuperJump()
{
    //Start the timer for the super jump
	GetWorldTimerManager().SetTimer(SuperJumpTimer, this, &ANetPlayerCharacter::StopJumping, ReleaseSuperJumpTime, false, ReleaseSuperJumpTime + SuperJumpTimerDelay);
    //Start the timer to slow down descent
	GetWorldTimerManager().SetTimer(SlowDescentTimer, this, &ANetPlayerCharacter::ServerForceStopAndSlowDescent, SuperJumpTimerDelay, false);
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Player Jumped");

    //If the player has started falling without jumping
    //Increase jump count
    if (GetCharacterMovement()->IsFalling() && JumpCurrentCount == 0)
        JumpCurrentCount++;
    //Increase jump count
	JumpCurrentCount++;

	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Jump Current Count: " + FString::FromInt(JumpCurrentCount));
}

void ANetPlayerCharacter::ReleaseSuperJump()
{
    //Check to see if we have slowed our descent
    //If true, do super jump
    //If false, do a normal jump
	if (GetWorldTimerManager().IsTimerActive(SlowDescentTimer) == false)
	{
        //Get the total time elapsed for the super jump timer
		float elapsedTime = GetWorldTimerManager().GetTimerElapsed(SuperJumpTimer);
        //Determine the power multiplier by taking the elapsedTime / MaxPowerSuperJumpTime + SuperJumpTimerDelay
        float powerScale = elapsedTime / MaxPowerSuperJumpTime;
        //If the scale goes above 1, set scale to 1
		if (powerScale >= 1.0f)
            powerScale = 1.0f;

        //Determine the max power of the jump by doing JumpZVelocity * MaxSuperJumpPowerScale
		float MaxSuperJump = JumpZVelocity * MaxSuperJumpPowerScale;

        //Determine the direction we are looking at as our direction to jump
		FVector cameraDirection = FirstPersonCamera->GetComponentRotation().Vector().GetSafeNormal();
        //Determine the base power of the jump
		FVector baseForce = cameraDirection * JumpZVelocity;
        //Determine the added power of the jump by
        //Taking the cameraDirection * StoredSpeedBeforeJump * (BasedSuperJumpMultiplier + AddedSuperJumpMultiplier * powerScale) 
		FVector storedForce = cameraDirection * StoredSpeedBeforeJump * (BaseSuperJumpMultiplier + AddedSuperJumpMultiplier * powerScale);
        //Add the baseForce and storedForce togeter
		FVector newForceVector = (baseForce + storedForce);

        //If the newForceVector exceeds the MaxSuperJump power
        //Use the MaxSuperJump power
		if (newForceVector.Size() > MaxSuperJump)
		{
            //Multiply the force by a small float value to deal with float rounding errors
            //Due to anti-cheat
			newForceVector = newForceVector.GetUnsafeNormal() * MaxSuperJump * 0.999997f;
		}

        //Stop the prepare super jump sound
        //By replacing it with a release super jump sound
        //If we don't have one, just stop the sound
		if (ReleaseSuperJumpSound)
		{
            ServerPlaySound(ReleaseSuperJumpSound);
		}
		else
            ServerPlaySound(nullptr);

        //Super jump
		ServerSuperJump(newForceVector);
	}
	else
	{
        //Cancel out Z velocity so that jumping will always be the same height
		float cancelVector = GetVelocity().Z;
		FVector jumpVector = GetActorUpVector() * (GetCharacterMovement()->JumpZVelocity - cancelVector);
		ServerPlaySound(JumpingSound);
		ServerSuperJump(jumpVector);
	}
}

void ANetPlayerCharacter::FireWeapon()
{
    //Pull the trigger of the weapon
	if (CurrentWeapon)
		CurrentWeapon->PullTrigger();
	//FRotator camRot = FirstPersonCamera->GetComponentRotation();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Gun Rotation - " + FString::SanitizeFloat(camRot.Yaw) + " , " + FString::SanitizeFloat(camRot.Pitch));
}

void ANetPlayerCharacter::StopFireWeapon()
{
    //Check to see if it's a local player is controlling the character
	if (!GetController()->IsLocalController())
		return;
	if (!CurrentWeapon)
		return;
    //Release the trigger of the weapon
	CurrentWeapon->ReleaseTrigger();
}

void ANetPlayerCharacter::ServerCameraUpdateRotation_Implementation(FRotator rot)
{
	//FRotator camRot = FirstPersonCamera->GetComponentRotation();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Gun Rotation - " + FString::SanitizeFloat(camRot.Yaw) + " , " + FString::SanitizeFloat(camRot.Pitch));

    //Set the camera rotation on the server to match client
	FirstPersonCamera->SetWorldRotation(rot);
}

bool ANetPlayerCharacter::ServerCameraUpdateRotation_Validate(FRotator rot)
{
	return true;
}

//Debug function
void ANetPlayerCharacter::ServerSpawnGuns_Implementation()
{
    if (GetWorld())
    {
        ATerminus_22XXGameModeBase* mode = Cast<ATerminus_22XXGameModeBase>(GetWorld()->GetAuthGameMode());
        if (mode)
            mode->SpawnStartingWeapons(this);
    }
}

bool ANetPlayerCharacter::ServerSpawnGuns_Validate()
{
    return true;
}

void ANetPlayerCharacter::NormalDescent()
{
    //Set the character's gravity value back to normal
	GetCharacterMovement()->GravityScale = BaseGravityScale;
}

void ANetPlayerCharacter::NetMulticastPlaySound_Implementation(class USoundBase* soundClip)
{
    //If no sound clip was sent, stop the sound
    if (soundClip == nullptr)
    {
        AudioComponent->Stop();
        return;
    }
    //Play a sound clip to everyone
    AudioComponent->Sound = soundClip;
    AudioComponent->Play();
}

void ANetPlayerCharacter::ServerAttachNewWeapon_Implementation(ANetBaseGun * nextGun)
{
    if (nextGun == nullptr)
        return;

    //If the gun exists and player is holding a weapon
    if (CurrentWeapon != nullptr)
    {
        //If the gun has a different weapon index than the current weapon
        //Switch guns
        if (nextGun->GetWeaponIndex() != CurrentWeapon->GetWeaponIndex())
        {
            //Stop the current gun's actions
            CurrentWeapon->ClientFullStop();
            //Detach gun from player
            CurrentWeapon->ServerDetach();
            //Switch the gun to the next gun
            CurrentWeapon = nextGun;
            //Attach the weapon
            CurrentWeapon->ServerAttach(this);
        }
    }
    //If the character is not holding a weapon
    else if (CurrentWeapon == nullptr)
    {
        //Set the current weapon to next gun and attach the weapon
        CurrentWeapon = nextGun;
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
    //Store the current speed only on the server
	if (Role == ROLE_Authority)
		StoreCurrentSpeed();
    //Stop the character's movements
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
    //Set the character gravity to a weaker one
	GetCharacterMovement()->GravityScale = WeakenedGravityScale;
}

bool ANetPlayerCharacter::NetMulticastForceStopAndSlowDescent_Validate()
{
	return true;
}
void ANetPlayerCharacter::ServerSuperJump_Implementation(FVector Impulse)
{
    //Add impulse to make the charater super jump
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
    //Make sure the super jump value doesn't exceed what's allowed
	if (Impulse.Size() > JumpZVelocity * MaxSuperJumpPowerScale)
		return false;
	else
		return true;
}

void ANetPlayerCharacter::ServerForceStopAndSlowDescent_Implementation()
{
    
	NetMulticastForceStopAndSlowDescent();

    //Play the charging sound
    if (ChargingSound)
    {
        NetMulticastPlaySound(ChargingSound);
    }
}

bool ANetPlayerCharacter::ServerForceStopAndSlowDescent_Validate()
{
	return true;
}

void ANetPlayerCharacter::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
    //Make player take damage
    if (GetPlayerState())
    {
        GetPlayerState()->CurrentHealth -= Damage;
        //If the character has died
        if (GetPlayerState()->CurrentHealth <= 0)
        {
            //Stop weapon functions and drop it
            CurrentWeapon->ClientFullStop();
            CurrentWeapon->ServerDetach();

            //Stop the player from taking input and colliding with others
            GetController()->DisableInput(Cast<APlayerController>(GetController()));
            GetCapsuleComponent()->SetSimulatePhysics(false);
            GetCapsuleComponent()->SetCollisionProfileName("NoCollision");

            //Start the respawn timer
            FTimerHandle RespawnTimer;
            GetWorldTimerManager().SetTimer(RespawnTimer, this, &ANetPlayerCharacter::Respawn, RespawnDelay, false);
        }
    }
}

float ANetPlayerCharacter::GetSuperJumpChargePercent()
{
    float timeElapsed = GetWorldTimerManager().GetTimerElapsed(SuperJumpTimer);
    float percent = timeElapsed / MaxPowerSuperJumpTime;
    return (percent > 1.0f) ? 1.0f : percent;
}

void ANetPlayerCharacter::StoreCurrentSpeed()
{
    //Store the current velocity of the player
	StoredSpeedBeforeJump = GetVelocity().Size();
}

bool ANetPlayerCharacter::AddHealth(float Amount)
{
    //If the character is already at max health
    //Don't add and health
	if (!GetPlayerState())
		return false;
    if (GetPlayerState()->CurrentHealth >= MaxHealth)
        return false;

    //Increase the amount of health by the amount
    GetPlayerState()->CurrentHealth += Amount;
    //If the current health restored exceeds the max health
    //set current health to max health
    if (GetPlayerState()->CurrentHealth >= MaxHealth)
        GetPlayerState()->CurrentHealth = MaxHealth;

    return true;
}

bool ANetPlayerCharacter::AddWeaponToInventory(ANetBaseGun * AGun)
{
    //Check if the weapon inventory succesfully adds the weapon
	if (WeaponInventory->AddWeapon(AGun))
	{
        //Set the owner of the gun to the player
		AGun->SetOwner(this);
        //Auto equip the new gun
		ServerAttachNewWeapon(WeaponInventory->SwitchToGun(AGun->GetWeaponIndex()));
        //Make player state keep track of owned guns
        if (GetPlayerState())
		    GetPlayerState()->CurrentGuns.Add(AGun);
		return true;
	}
	return false;
}

void ANetPlayerCharacter::Reload()
{
    //Prepare the reload of the gun
    if (CurrentWeapon)
    {
        CurrentWeapon->PrepareReload();
    }
}

void ANetPlayerCharacter::SwitchWeapon(float Val)
{
    //Switch weapon with the scroll wheel
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
    //Switch to next weapon using the scroll wheel
    if (!GetWorldTimerManager().IsTimerActive(WeaponSwitchTimer))
    {
        ServerAttachNewWeapon(WeaponInventory->NextWeapon());
        GetWorldTimerManager().SetTimer(WeaponSwitchTimer, WeaponSwitchLockout, false);
    }
}

void ANetPlayerCharacter::PreviousWeapon()
{
    //Switch to previous weapon using the scroll wheel
    if (!GetWorldTimerManager().IsTimerActive(WeaponSwitchTimer))
    {
        ServerAttachNewWeapon(WeaponInventory->PreviousWeapon());
        GetWorldTimerManager().SetTimer(WeaponSwitchTimer, WeaponSwitchLockout, false);
    }
}

void ANetPlayerCharacter::SwitchToShotgun()
{
    //Directly switch to shotgun
    ServerAttachNewWeapon(WeaponInventory->GetShotgun());
}

void ANetPlayerCharacter::SwitchToVampyr()
{
    //Directly switch to Vaympyr
    ServerAttachNewWeapon(WeaponInventory->GetVampyr());
}

void ANetPlayerCharacter::SwitchToRailgun()
{
    //Directly switch to railgun
    ServerAttachNewWeapon(WeaponInventory->GetRailgun());
}

void ANetPlayerCharacter::LogIn()
{
	if (Role == ROLE_Authority)
	{
        //Make the game state keep track of players
		GetGameState()->ConnectedPlayers.Add(PlayerState);
		GetGameState()->PlayerReachedBoss.Add(false);
        //Set the player state's max health to the actor's max health
		GetPlayerState()->MaxHealth = MaxHealth;
        GetPlayerState()->CurrentHealth = MaxHealth;
	}
}

void ANetPlayerCharacter::Respawn()
{
    //Respawn only if it's the server
    if (Role == ROLE_Authority)
    {
        ATerminus_22XXGameModeBase* mode = Cast<ATerminus_22XXGameModeBase>(GetGameState()->AuthorityGameMode);
        if (mode)
        {
            mode->RespawnPlayer(Cast<APlayerController>(GetController()));
            //Re-enable input of the player
            GetController()->EnableInput(Cast<APlayerController>(GetController()));
        }
    }
    //If it's not the server, call server respawn
    if (Role < ROLE_Authority)
    {
        ServerRespawn();
    }
}

void ANetPlayerCharacter::SetPlayerState(APlayerState * state)
{
    //Take an old player state and set it to this
    //player's player state
    PlayerState = state;
    //Grab the guns that are stored in the player state
    TArray<ANetBaseGun*> guns = GetPlayerState()->CurrentGuns;
    //Empty it so we will re-add them
	GetPlayerState()->CurrentGuns.Empty();
    for (int i = 0; i < guns.Num(); i++)
    {
        //Re-add the guns from the player state to the
        //inventory and player state
        AddWeaponToInventory(guns[i]);
    }
    //Grab the first weapon and equip it
    ServerAttachNewWeapon(WeaponInventory->GetAWeapon());
    //Restore the player's state current health to max health
    GetPlayerState()->CurrentHealth = MaxHealth;
}

void ANetPlayerCharacter::SetActiveCheckpoint(int CheckpointID)
{
	if (GetPlayerState())
	{
        //Set the respawn point to the new checkpoint
		GetPlayerState()->CurrentCheckpointID = CheckpointID;
	}
}

void ANetPlayerCharacter::PlayerReachedFinale()
{
	for (int i = 0; i < GetGameState()->ConnectedPlayers.Num(); i++)
	{
		//Find the matching player state in the game state
		if (GetGameState()->ConnectedPlayers[i] == PlayerState)
		{
			if (!GetGameState()->PlayerReachedBoss[i])
			{
				//Tell gamestate that the player has successfully reached the boss
				GetGameState()->PlayerReachedBoss[i] = true;
				//Start the boss countdown
				GetGameState()->StartBossCountDown = true;
				//Record the time they reached the boss
				GetPlayerState()->TimeWhenBossReached = GetGameState()->CurrentGameTime;
			}
		}
	}
}

ANetPlayerState * ANetPlayerCharacter::GetPlayerState()
{
    //Get the Player state
    return  Cast<ANetPlayerState>(PlayerState);
}

ATerminus_22XX_GameState* ANetPlayerCharacter::GetGameState()
{
    //Get the game state
    return Cast<ATerminus_22XX_GameState>(GetWorld()->GetGameState());
}

void ANetPlayerCharacter::ServerRespawn_Implementation()
{
	Respawn();
}
bool ANetPlayerCharacter::ServerRespawn_Validate()
{
	return true;
}

void ANetPlayerCharacter::ServerPlaySound_Implementation(USoundBase * soundClip)
{
	NetMulticastPlaySound(soundClip);
}

bool ANetPlayerCharacter::ServerPlaySound_Validate(USoundBase * soundClip)
{
	return true;
}

void ANetPlayerCharacter::ServerAddScore_Implementation(float Score)
{
    //Increase the player score
	PlayerState->Score += Score;
}

bool ANetPlayerCharacter::ServerAddScore_Validate(float Score)
{
	return true;
}

void ANetPlayerCharacter::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //Replicate the replicated variables
    DOREPLIFETIME_CONDITION(ANetPlayerCharacter, StoredSpeedBeforeJump, COND_OwnerOnly);
    DOREPLIFETIME(ANetPlayerCharacter, CurrentWeapon);

}
