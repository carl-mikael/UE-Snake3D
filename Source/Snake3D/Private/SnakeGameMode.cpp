// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"

#include "SnakeGameState.h"
#include "SnakePawn.h"
#include "SnakePlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ASnakeGameMode::RegisterSnakePawn(ASnakePawn* SnakePawn)
{
	if (!IsValid(SnakePawn))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeGameMode::RegisterSnakePawn() - SnakePawn is invalid"));
		return;
	}
	
	SnakePawn->OnSnakeHit.AddDynamic(this, &ASnakeGameMode::OnSnakeHit);
}

void ASnakeGameMode::UnRegisterSnakePawn(ASnakePawn* SnakePawn)
{
	if (!IsValid(SnakePawn))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeGameMode::UnRegisterSnakePawn() - SnakePawn is invalid"));
		return;
	}
	
	SnakePawn->OnSnakeHit.RemoveDynamic(this, &ASnakeGameMode::OnSnakeHit);
}

void ASnakeGameMode::InitiateNextStage() const
{
	int NewStage = GetGameState<ASnakeGameState>()->NextStage();
	
	// Reset Scores
	for (const auto PlayerState : this->GameState->PlayerArray)
	{
		PlayerState->SetScore(0);
	}
}

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ASnakeGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	const bool bIsHost = Player->IsLocalController();
		
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	
	AActor** PlayerStart = PlayerStarts.FindByPredicate([&bIsHost](AActor* Start)
	{
		return Cast<APlayerStart>(Start)->PlayerStartTag == (bIsHost? TEXT("P1") : TEXT("P2"));
	});
	
	return *PlayerStart;
}

// WARNING: works for 2 players only!
// Other player wins if one crashes.
// If both players crash into each other, Server wins.
void ASnakeGameMode::OnSnakeHit(ASnakePawn* SnakePawn, ESnakeCollision CollisionType)
{
	UE_LOG(LogTemp, Log, TEXT("SnakeGameMode::OnSnakeHit() - Hit something!"));
	
	if (GetGameState<ASnakeGameState>()->IsStageWon() || !IsValid(SnakePawn))
	{
		if (!IsValid(SnakePawn))
		{
			UE_LOG(LogTemp, Error, TEXT("SnakeGameMode::OnSnakeHit() - Invalid SnakePawn"));
		}
		
		return;
	}
	
	switch (CollisionType)
	{
	case ESnakeCollision::ASnakeHead:
		WinStage(Cast<ASnakePlayerState>(this->GameState->PlayerArray[0]));
		break;
	case ESnakeCollision::ASnakeBodyCell:
		for (const auto PlayerState : this->GameState->PlayerArray)
		{
			// The player state that doesn't own the pawn that collided wins.
			if (PlayerState->GetPawn() != SnakePawn || NumPlayers == 1)
			{
				WinStage(Cast<ASnakePlayerState>(PlayerState));
			}
		}
		break;
	case ESnakeCollision::AFood:
		ASnakePlayerState* PState = Cast<ASnakePlayerState>(SnakePawn->GetPlayerState());
		if (IsValid(PState))
		{
			PState->SetScore(PState->GetScore() + 1);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - Score updated!"));
			UE_LOG(LogTemp, Log, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - New Score: %f"), PState->GetScore());
			
			if (PState->GetScore() >= Points_Needed_To_Win_Stage)
			{
				WinStage(PState);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
		}
		break;
	}
}

void ASnakeGameMode::WinStage(ASnakePlayerState* WinningPlayerState) const
{
	GetGameState<ASnakeGameState>()->SetStageWon(true);
	
	// Won Game
	if (WinningPlayerState->AddWin() >= Stages_Needed_To_Win_Game)
	{
		OnGameWon.Broadcast(WinningPlayerState);
	}
	// Won Stage
	else
	{
		OnStageWon.Broadcast(WinningPlayerState, WinningPlayerState->GetScore());
	}
}
