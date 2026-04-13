// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "LevelSwitcher.h"
#include "SteamSession.h"

void UMainMenu::SwitchToLevel(const TSoftObjectPtr<UWorld> LevelAsset) const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		const ULevelSwitcher* LevelSwitcherSubsystem = GameInstance->GetSubsystem<ULevelSwitcher>();
		if (IsValid(LevelSwitcherSubsystem))
		{
			LevelSwitcherSubsystem->SwitchToLevel(LevelAsset);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MainMenu::SwitchToLevel() - LevelSwitcherSubsystem is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenu::SwitchToLevel() - GameInstance is null"));
	}
}

void UMainMenu::CreateSession(const FName SessionName) const
{
	USteamSession* SteamSesh = GetGameInstance()->GetSubsystem<USteamSession>();
	if (!IsValid(SteamSesh))
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenu::CreateSession() - SteamSesh is null"));
		return;
	}
	
	SteamSesh->Host();
}
