#include "GameManager.h"
#include "CosmicRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.TickInterval = 0.016f;
	PrimaryActorTick.bCanEverTick = true;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeGame();
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentGameState)
	{
		case EGameState::Playing:
			UpdateGameplay(DeltaTime);
			break;
		case EGameState::Paused:
		case EGameState::GameOver:
		case EGameState::MainMenu:
		default:
			break;
	}
}

void AGameManager::StartGame()
{
	if (CurrentGameState != EGameState::Playing)
	{
		EGameState OldState = CurrentGameState;
		CurrentGameState = EGameState::Playing;
		ElapsedTime = 0.0f;
		CurrentDifficulty = 1.0f;

		if (PlayerCharacter)
		{
			PlayerCharacter->SetActorLocation(FVector(0, 0, 100));
		}

		UGameplayStatics::SetGamePaused(GetWorld(), false);
		OnGameStateChanged.Broadcast(CurrentGameState);
	}
}

void AGameManager::PauseGame()
{
	if (CurrentGameState == EGameState::Playing)
	{
		CurrentGameState = EGameState::Paused;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		OnGameStateChanged.Broadcast(CurrentGameState);
	}
}

void AGameManager::ResumeGame()
{
	if (CurrentGameState == EGameState::Paused)
	{
		CurrentGameState = EGameState::Playing;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		OnGameStateChanged.Broadcast(CurrentGameState);
	}
}

void AGameManager::GameOver()
{
	CurrentGameState = EGameState::GameOver;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	SaveHighScore();
	OnGameStateChanged.Broadcast(CurrentGameState);
}

void AGameManager::RestartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()), false);
}

void AGameManager::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("MainMenu")), false);
}

int32 AGameManager::GetCurrentScore() const
{
	if (PlayerCharacter)
	{
		return PlayerCharacter->GetScore();
	}
	return 0;
}

void AGameManager::UpdateDifficulty(float DeltaTime)
{
	if (CurrentGameState == EGameState::Playing)
	{
		float NewDifficulty = FMath::Min(CurrentDifficulty + DifficultyIncreaseRate * DeltaTime, MaxDifficulty);
		if (NewDifficulty != CurrentDifficulty)
		{
			CurrentDifficulty = NewDifficulty;
			OnDifficultyChanged.Broadcast(CurrentDifficulty);
		}
	}
}

void AGameManager::InitializeGame()
{
	LoadHighScore();
	PlayerCharacter = Cast<ACosmicRunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AGameManager::UpdateGameplay(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	UpdateDifficulty(DeltaTime);

	// Check if player is dead
	if (PlayerCharacter && PlayerCharacter->GetHealth() <= 0.0f)
	{
		GameOver();
	}
}

void AGameManager::SaveHighScore()
{
	int32 CurrentScore = GetCurrentScore();
	if (CurrentScore > HighScore)
	{
		HighScore = CurrentScore;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		// TODO: Implement actual save system
	}
}

void AGameManager::LoadHighScore()
{
	// TODO: Implement actual load system
	HighScore = 0;
}