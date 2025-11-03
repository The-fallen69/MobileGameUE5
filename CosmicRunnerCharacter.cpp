#include "CosmicRunnerCharacter.h"
#include "Camera/CameraBoom.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ACosmicRunnerCharacter::ACosmicRunnerCharacter()
{
	PrimaryActorTick.TickInterval = 0.016f;
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character with camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;

	// Create camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CurrentHealth = MaxHealth;
}

void ACosmicRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentLane = 1;
	CurrentState = ECharacterState::Running;
}

void ACosmicRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Auto-run forward
	AddMovementInput(GetActorForwardVector(), 1.0f);

	// Update character state
	UpdateCharacterState();
}

void ACosmicRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		// Add input for lane movement
		PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ACosmicRunnerCharacter::MoveLeft);
		PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ACosmicRunnerCharacter::MoveRight);
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACosmicRunnerCharacter::Jump);
	}
}

void ACosmicRunnerCharacter::MoveLeft()
{
	if (CurrentLane > 0)
	{
		CurrentLane--;
		HandleLaneMovement(-1.0f);
	}
}

void ACosmicRunnerCharacter::MoveRight()
{
	if (CurrentLane < 2)
	{
		CurrentLane++;
		HandleLaneMovement(1.0f);
	}
}

void ACosmicRunnerCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		Super::Jump();
		CurrentState = ECharacterState::Jumping;

		if (JumpSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), JumpSound, GetActorLocation());
		}

		OnCharacterStateChanged.Broadcast(CurrentState, ECharacterState::Running);
	}
}

void ACosmicRunnerCharacter::TakeDamage(float Damage)
{
	if (!bIsInvulnerable)
	{
		float OldHealth = CurrentHealth;
		CurrentHealth = FMath::Max(0.0f, CurrentHealth - Damage);

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}

		OnHealthChanged.Broadcast(CurrentHealth, OldHealth);

		if (CurrentHealth <= 0.0f)
		{
			Die();
		}
		else
		{
			CurrentState = ECharacterState::Hit;
		}
	}
}

void ACosmicRunnerCharacter::ApplyPowerUp(FString PowerUpType, float Duration)
{
	if (PowerUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PowerUpSound, GetActorLocation());
	}

	if (PowerUpType == TEXT("Invulnerability"))
	{
		bIsInvulnerable = true;
		PowerUpDuration = Duration;
		GetWorldTimerManager().SetTimer(PowerUpTimerHandle, [this]() { RemovePowerUp(TEXT("Invulnerability")); }, Duration, false);
	}
	else if (PowerUpType == TEXT("SpeedBoost"))
	{
		bIsSpeedBoosted = true;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * SpeedBoostMultiplier;
		PowerUpDuration = Duration;
		GetWorldTimerManager().SetTimer(PowerUpTimerHandle, [this]() { RemovePowerUp(TEXT("SpeedBoost")); }, Duration, false);
	}
}

void ACosmicRunnerCharacter::Die()
{
	CurrentState = ECharacterState::Dead;
	GetCharacterMovement()->DisableMovement();
	OnCharacterStateChanged.Broadcast(CurrentState, ECharacterState::Hit);
}

void ACosmicRunnerCharacter::UpdateCharacterState()
{
	ECharacterState NewState = CurrentState;

	if (CurrentHealth <= 0.0f)
	{
		NewState = ECharacterState::Dead;
	}
	else if (!GetCharacterMovement()->IsMovingOnGround())
	{
		NewState = ECharacterState::Jumping;
	}
	else if (CurrentState != ECharacterState::Hit)
	{
		NewState = ECharacterState::Running;
	}

	if (NewState != CurrentState)
	{
		ECharacterState OldState = CurrentState;
		CurrentState = NewState;
		OnCharacterStateChanged.Broadcast(NewState, OldState);
	}
}

void ACosmicRunnerCharacter::HandleLaneMovement(float Direction)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Y += Direction * LaneWidth;
	SetActorLocation(NewLocation);
}

void ACosmicRunnerCharacter::UpdateScore(int32 Amount)
{
	int32 OldScore = CurrentScore;
	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);
}

void ACosmicRunnerCharacter::RemovePowerUp(FString PowerUpType)
{
	if (PowerUpType == TEXT("Invulnerability"))
	{
		bIsInvulnerable = false;
	}
	else if (PowerUpType == TEXT("SpeedBoost"))
	{
		bIsSpeedBoosted = false;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
}