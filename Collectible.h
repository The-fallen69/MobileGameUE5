#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectible.generated.h"

class ACosmicRunnerCharacter;

UENUM(BlueprintType)
enum class ECollectibleType : uint8
{
	Coin UMETA(DisplayName = "Coin"),
	Gem UMETA(DisplayName = "Gem"),
	Shield UMETA(DisplayName = "Shield"),
	SpeedBoost UMETA(DisplayName = "Speed Boost")
};

UCLASS()
class COSMICRUNNER_API ACollectible : public AActor
{
	GENERATED_BODY()

public:
	ACollectible();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Properties
	UFUNCTION(BlueprintCallable, Category = "Collectible")
	void SetCollectibleType(ECollectibleType NewType) { CollectibleType = NewType; }

	UFUNCTION(BlueprintCallable, Category = "Collectible")
	ECollectibleType GetCollectibleType() const { return CollectibleType; }

	UFUNCTION(BlueprintCallable, Category = "Collectible")
	int32 GetPointValue() const { return PointValue; }

	UFUNCTION(BlueprintCallable, Category = "Collectible")
	void Collect();

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class URotatingMovementComponent* RotationComponent;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Type")
	ECollectibleType CollectibleType = ECollectibleType::Coin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Value")
	int32 PointValue = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Movement")
	float MovementSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Visuals")
	FLinearColor CollectibleColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Audio")
	class USoundBase* CollectSound;

	UPROPERTY(BlueprintReadWrite, Category = "Collectible|State")
	bool bHasBeenCollected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible|Animation")
	float RotationSpeed = 180.0f;

private:
	void UpdatePosition(float DeltaTime);
	void PlayCollectEffect();

public:
	// Delegates
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCollectibleCollected, ACollectible*, Collectible, int32, PointValue);
	UPROPERTY(BlueprintAssignable, Category = "Collectible|Events")
	FOnCollectibleCollected OnCollected;
};
