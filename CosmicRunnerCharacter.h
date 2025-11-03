#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CosmicRunnerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Running UMETA(DisplayName = "Running"),
	Jumping UMETA(DisplayName = "Jumping"),
	Hit UMETA(DisplayName = "Hit"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class COSMICRUNNER_API ACosmicRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACosmicRunnerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Character Movement
	UFUNCTION(BlueprintCallable, Category = "Character")
	void MoveLeft();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void MoveRight();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void Jump();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void TakeDamage(float Damage);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ApplyPowerUp(FString PowerUpType, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void Die();

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	ECharacterState GetCharacterState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	int32 GetScore() const { return CurrentScore; }

protected:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// Character stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Character|Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Stats")
	float MovementSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Stats")
	float JumpForce = 800.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Character|Stats")
	int32 CurrentScore = 0;

	// State
	UPROPERTY(BlueprintReadWrite, Category = "Character|State")
	ECharacterState CurrentState = ECharacterState::Running;

	// Lane management
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement")
	float LaneWidth = 200.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Character|Movement")
	int32 CurrentLane = 1; // 0 = left, 1 = center, 2 = right

	// Power-ups
	UPROPERTY(BlueprintReadWrite, Category = "PowerUp")
	bool bIsInvulnerable = false;

	UPROPERTY(BlueprintReadWrite, Category = "PowerUp")
	bool bIsSpeedBoosted = false;

	UPROPERTY(BlueprintReadWrite, Category = "PowerUp")
	float SpeedBoostMultiplier = 1.5f;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* RunAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* JumpAnimation;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* PowerUpSound;

private:
	void UpdateCharacterState();
	void HandleLaneMovement(float Direction);
	void UpdateScore(int32 Amount);
	void RemovePowerUp(FString PowerUpType);

	FTimerHandle PowerUpTimerHandle;
	float PowerUpDuration = 0.0f;

public:
	// Delegate for state changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterStateChanged, ECharacterState, NewState, ECharacterState, OldState);
	UPROPERTY(BlueprintAssignable, Category = "Character|Events")
	FCharacterStateChanged OnCharacterStateChanged;

	// Delegate for health changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChanged, float, NewHealth, float, OldHealth);
	UPROPERTY(BlueprintAssignable, Category = "Character|Events")
	FHealthChanged OnHealthChanged;

	// Delegate for score changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChanged, int32, NewScore);
	UPROPERTY(BlueprintAssignable, Category = "Character|Events")
	FScoreChanged OnScoreChanged;
};
