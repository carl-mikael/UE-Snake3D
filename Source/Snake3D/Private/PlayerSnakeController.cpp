// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSnakeController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APlayerSnakeController::BeginPlay()
{
    Super::BeginPlay();
    
    if (InputMappingContext.Get() == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - InputMappingContext is nullptr"))
    }
    
    if (TurnAction.Get() == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - TurnAction is nullptr"))
    }

    const ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!IsValid(LocalPlayer))
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

// ReSharper disable once CppMemberFunctionMayBeConst
void APlayerSnakeController::HandleTurn(const FInputActionValue& Value)
{
    const float TurnValue = Value.Get<float>();
    UE_LOG(LogTemp, Log, TEXT("PlayerSnakeController::HandleTurn - Turn input: %f"), TurnValue);
    
    const FVector InputMove(0.0f, 0.0f, TurnValue);
    APawn* TmpPawn = GetPawn().Get();
    if (IsValid(TmpPawn))
    {
        TmpPawn->AddMovementInput(InputMove);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::HandleTurn - Pawn is invalid"));
    }
}

