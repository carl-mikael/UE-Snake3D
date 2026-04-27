// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSnakeController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "SnakeGameMode.h"
#include "GameFramework/PlayerState.h"

void APlayerSnakeController::BeginPlay()
{
    Super::BeginPlay();
    
    ASnakeGameMode* SnakeGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASnakeGameMode>() : nullptr;
    if (!IsValid(SnakeGameMode))
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay() - SnakeGameMode invalid"));
        return;
    }
    SnakeGameMode->OnWinnerDelegate.AddDynamic(this, &APlayerSnakeController::Client_OnPlayerStateWin);
}

void APlayerSnakeController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    
    if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (!InputMappingContext.IsNull())
            {
                constexpr int Priority = 0;
                InputSystem->AddMappingContext(InputMappingContext.LoadSynchronous(), Priority);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - Imc is null"))
                return;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - InputSystem is null"))
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay - LocalPlayer is null"))
        return;
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("PlayerSnakeController::OnPossess - Input mapping context added without problem"));
}

void APlayerSnakeController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &APlayerSnakeController::HandleTurn);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerSnakeController::SetupInputComponent - EnhancedInputComponent is invalid"));
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

void APlayerSnakeController::OnPlayerStateWin(APlayerState* WinningState)
{
    Client_OnPlayerStateWin(WinningState);
}

void APlayerSnakeController::Client_OnPlayerStateWin_Implementation(APlayerState* WinningState)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s won."), WinningState->GetOwningController()==this? TEXT("You") : TEXT("Other player")));
    OnPlayerWin(WinningState);
}
