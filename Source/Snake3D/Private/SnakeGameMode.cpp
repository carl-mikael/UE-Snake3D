// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"

#include "Food.h"
#include "SnakePawn.h"
#include "SnakePlayerState.h"
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
	if (IsValid(SnakePawn) && OtherActor->IsA(AFood::StaticClass()))
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
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
		}
	}
}
