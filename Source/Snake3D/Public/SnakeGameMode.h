// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SnakeGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerStateWinnerDelegate, APlayerState*, WinningState, float, Score);

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
	
protected:
	constexpr static int Points_Needed_To_Win = 10;
	bool bIsGameWon = false;
	
public:
	UPROPERTY()
	FPlayerStateWinnerDelegate OnWinnerDelegate;
	
	void RegisterSnakePawn(ASnakePawn* SnakePawn);
	void UnRegisterSnakePawn(ASnakePawn* SnakePawn);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnSnakeHit(ASnakePawn* SnakePawn, ESnakeCollision CollisionType);
};
