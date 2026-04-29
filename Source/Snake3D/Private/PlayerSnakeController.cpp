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
    
    if (HasAuthority())
    {
        ASnakeGameMode* SnakeGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASnakeGameMode>() : nullptr;
        if (!IsValid(SnakeGameMode))
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::BeginPlay() - SnakeGameMode invalid"));
            return;
        }
        
        SnakeGameMode->OnStageWon.AddDynamic(this, &APlayerSnakeController::Client_OnPlayerStageWin);
        SnakeGameMode->OnGameWon.AddDynamic(this, &APlayerSnakeController::Client_OnPlayerGameWin);
        UE_LOG(LogTemp, Log, TEXT("PlayerSnakeController::BeginPlay() - Subscribed to OnWinDelegates"));
    }
}

void APlayerSnakeController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (HasAuthority())
    {
        ASnakeGameMode* SnakeGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASnakeGameMode>() : nullptr;
        if (!IsValid(SnakeGameMode))
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::EndPlay() - SnakeGameMode invalid"));
            return;
        }
        
        SnakeGameMode->OnStageWon.RemoveDynamic(this, &APlayerSnakeController::Client_OnPlayerStageWin);
        SnakeGameMode->OnGameWon.RemoveDynamic(this, &APlayerSnakeController::Client_OnPlayerGameWin);
        UE_LOG(LogTemp, Log, TEXT("PlayerSnakeController::EndPlay() - Unsubscribed to OnWinDelegates"));
    }
}

void APlayerSnakeController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    Client_OnPossess(InPawn);
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

void APlayerSnakeController::Client_OnPossess_Implementation(APawn* InPawn)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("PlayerSnakeController::Client_OnPossess - Trying to add input mapping"));
    
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
                UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::Client_OnPossess - Imc is null"))
                return;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::Client_OnPossess - InputSystem is null"))
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSnakeController::Client_OnPossess - LocalPlayer is null"))
        return;
    }
    
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("PlayerSnakeController::Client_OnPossess - Input mapping context added without problem"));
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

void APlayerSnakeController::Client_OnPlayerGameWin_Implementation(APlayerState* WinnerState)
{
    OnPlayerGameWin(WinnerState);
}

void APlayerSnakeController::Client_OnPlayerStageWin_Implementation(APlayerState* WinningState, const float Score)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s won."), WinningState->GetOwningController()==this? TEXT("You") : TEXT("Other player")));
    OnPlayerStageWin(WinningState, Score);
}
