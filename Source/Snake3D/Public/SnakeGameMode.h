// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SnakeGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerStateWinnerDelegate, APlayerState*, WinningState);

class ASnakePawn;
/**
 * 
 */
UCLASS()
class SNAKE3D_API ASnakeGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	constexpr static int Points_Needed_To_Win = 1;
	
public:
	UPROPERTY()
	FPlayerStateWinnerDelegate OnWinnerDelegate;
	void RegisterSnakePawn(ASnakePawn* SnakePawn);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnSneakHit(ASnakePawn* SnakePawn, AActor* OtherActor);
};
