// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSnakeController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APlayerSnakeController::BeginPlay()
{
    Super::BeginPlay();
    
    ControlledPawn = GetPawn();
    
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer->IsValidLowLevel())
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - Failed to get LocalPlayer"));
    }
    else if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - Failed to get EnhancedInputLocalPlayerSubsystem"));
    }
}

void APlayerSnakeController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &APlayerSnakeController::HandleTurn);
    }
}

void APlayerSnakeController::HandleTurn(const FInputActionValue& Value)
{
    const float TurnValue = Value.Get<float>();
    UE_LOG(LogTemp, Log, TEXT("PlayerSnakeController::HandleTurn - Turn input: %f"), TurnValue);
    
    ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), TurnValue);
}

