// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayerSnakeController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class SNAKE3D_API APlayerSnakeController : public APlayerController
{
	GENERATED_BODY()

	// --- Properties ---
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TurnAction;

	// --- Methods ---
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
	UFUNCTION(Client, Reliable)
	void Client_OnGameStageChanged(int NewGameStage);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameStageChanged(int NewGameStage);
	
	UFUNCTION(Client, Reliable)
	void Client_OnPossess(APawn* InPawn);
	
	UFUNCTION()
	void HandleTurn(const FInputActionValue& Value);
	
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerStageWin(APlayerState* WinnerState, float Score);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerStageWin(APlayerState* WinnerState, float Score);
	
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerGameWin(APlayerState* WinnerState);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerGameWin(APlayerState* WinnerState);
};
