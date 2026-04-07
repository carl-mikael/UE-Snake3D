// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSwitcher.h"
#include <Kismet/GameplayStatics.h>

void ULevelSwitcher::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("LevelSwitcherSubsystem - BeginPlay!!"));
}

void ULevelSwitcher::SwitchToLevel(const TSoftObjectPtr<UWorld> LevelAsset) const
{
	if (LevelAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LevelSwitcher - SwitchToLevel was called with null as Level asset"));
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelAsset);
}
