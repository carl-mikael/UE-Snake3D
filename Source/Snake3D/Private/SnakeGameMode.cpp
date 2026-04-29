// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"

#include "Food.h"
#include "SnakeBodyCell.h"
#include "SnakePawn.h"
#include "SnakePlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

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

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
// WARNING: works for 2 players only!
// Other player wins if one crashes
// If both crash into each other, Server wins
void ASnakeGameMode::OnSnakeHit(ASnakePawn* SnakePawn, ESnakeCollision CollisionType)
{
	UE_LOG(LogTemp, Log, TEXT("SnakeGameMode::OnSnakeHit() - Hit something!"));
	
	if (bIsGameWon || !IsValid(SnakePawn))
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
		bIsGameWon = true;
		{
			APlayerState* WinningState = this->GameState->PlayerArray[0];
			OnWinnerDelegate.Broadcast(WinningState, WinningState->GetScore());
		}
		break;
	case ESnakeCollision::ASnakeBodyCell:
		for (const auto PlayerState : this->GameState->PlayerArray)
		{
			// The player state that doesn't own the pawn that collided wins.
			if (PlayerState->GetPawn() != SnakePawn || NumPlayers == 1)
			{
				bIsGameWon = true;
				APlayerState* WinningState = PlayerState; 
				OnWinnerDelegate.Broadcast(WinningState, WinningState->GetScore());
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
			
			if (PState->GetScore() >= Points_Needed_To_Win)
			{
				bIsGameWon = true;
				APlayerState* WinningState = PState; 
				OnWinnerDelegate.Broadcast(WinningState, WinningState->GetScore());
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
