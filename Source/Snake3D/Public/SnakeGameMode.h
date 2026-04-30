// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameState.h"
#include "GameFramework/GameMode.h"
#include "SnakeGameMode.generated.h"

class ASnakePlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerStageWinnerDelegate, APlayerState*, WinningState, float, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerGameWinnerDelegate, APlayerState*, WinningState);

UENUM()
enum class ESnakeCollision
{
	ASnakeHead,
	ASnakeBodyCell,
	AFood
};

class ASnakePawn;
/**
 * 
 */
UCLASS()
class SNAKE3D_API ASnakeGameMode : public AGameMode
{
	GENERATED_BODY()
	
	// --- Properties ---
protected:
	constexpr static int Points_Needed_To_Win_Stage = 10;
	// BO3
	constexpr static int Stages_Needed_To_Win_Game = 2;
	
	constexpr static int Initial_Map_Size = 20;
	constexpr static int Map_Increase_Per_Stage = 5;
	
public:
	UPROPERTY()
	FPlayerStageWinnerDelegate OnStageWon;
	
	UPROPERTY()
	FPlayerGameWinnerDelegate OnGameWon;
	
	int GetMapSize() const
	{
		return Initial_Map_Size + Map_Increase_Per_Stage * GetGameState<ASnakeGameState>()->GetGameStage();
	}
	
	// --- Methods ---
public:
	void RegisterSnakePawn(ASnakePawn* SnakePawn);
	void UnRegisterSnakePawn(ASnakePawn* SnakePawn);
	
	UFUNCTION(BlueprintCallable)
	void InitiateNextStage();
	
protected:
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
private:
	// Has to be UFUNCTION to be able to bind it to event
	UFUNCTION()
	void OnSnakeHit(ASnakePawn* SnakePawn, ESnakeCollision CollisionType);
	
	void WinStage(ASnakePlayerState* WinningPlayerState) const;
};
