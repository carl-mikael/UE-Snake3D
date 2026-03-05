// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly());
			this->bShowMouseCursor = true;
		}
	}
}

void AMainMenuController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SetInputMode(FInputModeGameOnly());
	this->bShowMouseCursor = false;
}
