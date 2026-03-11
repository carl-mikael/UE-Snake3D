// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSnakeController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void APlayerSnakeController::BeginPlay()
{
    Super::BeginPlay();
    
    if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (!InputMappingContext.IsNull())
            {
                constexpr int Priority = 0;
                InputSystem->AddMappingContext(InputMappingContext.LoadSynchronous(), Priority);
            }
        }
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
