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
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TurnAction;

	// --- Methods ---
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void HandleTurn(const FInputActionValue& Value);
};
