// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSwitcherSubsystem.h"
#include <Kismet/GameplayStatics.h>

void ULevelSwitcherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("LevelSwitcherSubsystem - BeginPLay!!"));
}

void ULevelSwitcherSubsystem::SwitchLevel(TSoftObjectPtr<UWorld> LevelAsset)
{
	if (LevelAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LevelSwitcher - SwitchLevel was called with null as Level asset"));
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelAsset);
}
