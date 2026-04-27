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
	
	SnakePawn->OnSneakHit.AddDynamic(this, &ASnakeGameMode::OnSneakHit);
}

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASnakeGameMode::OnSneakHit(ASnakePawn* SnakePawn, AActor* OtherActor)
{
	if (bIsGameWon || !IsValid(SnakePawn))
	{
		return;
	}
	
	if (OtherActor->IsA(AFood::StaticClass()))
	{
		ASnakePlayerState* PState = Cast<ASnakePlayerState>(SnakePawn->GetPlayerState());
		if (IsValid(PState))
		{
			PState->SetScore(PState->GetScore() + 1);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - Score updated!"));
			UE_LOG(LogTemp, Log, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - New Score: %f"), PState->GetScore());
			
			if (PState->GetScore() >= Points_Needed_To_Win)
			{
				OnWinnerDelegate.Broadcast(PState);
				if (PState->GetScore() >= Points_Needed_To_Win)
				{
					bIsGameWon = true;
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
		}
	}
	else if (OtherActor->IsA(ASnakeBodyCell::StaticClass()))
	{
		for (const auto PlayerState : this->GameState->PlayerArray)
		{
			// The player state that doesn't own the pawn that collided wins.				WARNING: works for 2 players only!
			if (PlayerState->GetPawn() != SnakePawn || NumPlayers == 1)
			{
				bIsGameWon = true;
				OnWinnerDelegate.Broadcast(PlayerState);
			}
		}
	}
}
