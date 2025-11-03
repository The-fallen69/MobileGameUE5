#include "Obstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CosmicRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AObstacle::AObstacle()
{
	PrimaryActorTick.TickInterval = 0.016f;
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionEnabled(ECC_WorldDynamic);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECC_NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	
	// Set up collision delegates
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::NotifyActorBeginOverlap);
	}
}

void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePosition(DeltaTime);

	// Destroy if too far behind camera
	if (GetActorLocation().X < -1000.0f)
	{
		Destroy();
	}
}

void AObstacle::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ACosmicRunnerCharacter* Player = Cast<ACosmicRunnerCharacter>(OtherActor);
	if (Player && !bHasCollided)
	{
		bHasCollided = true;
		OnObstacleHit.Broadcast(this, OtherActor);
		OnCollisionWithPlayer(Player);
	}
}

void AObstacle::Destroy(float DelayTime)
{
	if (DelayTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			FTimerHandle(),
			[this]() { Super::Destroy(); },
			DelayTime,
			false
		);
	}
	else
	{
		Super::Destroy();
	}
}

void AObstacle::UpdatePosition(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.X -= MovementSpeed * DeltaTime;
	SetActorLocation(NewLocation);
	ElapsedTime += DeltaTime;
}

void AObstacle::OnCollisionWithPlayer(ACosmicRunnerCharacter* Player)
{
	if (Player)
	{
		Player->TakeDamage(Damage);

		if (CollisionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), CollisionSound, GetActorLocation());
		}

		// Slight knockback effect
		FVector KnockbackDirection = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		Player->LaunchCharacter(KnockbackDirection * 500.0f + FVector(0, 0, 300.0f), true, true);
	}
}