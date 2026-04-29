// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerState.h"
#include "Net/UnrealNetwork.h"

void ASnakePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASnakePlayerState, Wins);
}

void ASnakePlayerState::SetScore(const float NewScore)
{
	Super::SetScore(NewScore);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Score updated: %f"), GetScore()));
	OnScoreUpdated.Broadcast(NewScore);
}
