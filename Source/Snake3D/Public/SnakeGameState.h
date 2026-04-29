// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SnakeGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStageChanged, int, NewGameStage);

/**
 * 
 */
UCLASS()
class SNAKE3D_API ASnakeGameState : public AGameState
{
	GENERATED_BODY()
	
private:
	int GameStage = 1;
	bool bIsStageWon = false;
	
public:
	UPROPERTY()
	FOnGameStageChanged OnGameStageChanged;
	
	UFUNCTION(BlueprintCallable)
	int GetGameStage() const
	{
		return GameStage;
	}
	
	int NextStage()
	{
		GameStage++;
		bIsStageWon = false;
		OnGameStageChanged.Broadcast(GameStage);
		return GameStage;
	}
	
	bool IsStageWon() const
	{
		return bIsStageWon;
	}
	
	void SetStageWon(const bool bNewWon)
	{
		bIsStageWon = bNewWon;
	}
};
