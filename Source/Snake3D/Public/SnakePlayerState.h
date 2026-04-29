// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SnakePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreUpdated, int, NewScore);

/**
 * 
 */
UCLASS()
class SNAKE3D_API ASnakePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FScoreUpdated OnScoreUpdated;
	
private:
	UPROPERTY(Replicated)
	int Wins = 0;
	
public:
	// ReSharper disable once CppHidingFunction
	virtual void SetScore(const float NewScore);
	
	// Call only from Server
	UFUNCTION(BlueprintCallable)
	int AddWin()
	{
		Wins++;
		return Wins;
	};
	
	UFUNCTION(BlueprintCallable)
	int GetWins() const
	{
		return Wins;
	};
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
