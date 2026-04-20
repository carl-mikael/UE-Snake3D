// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSwitcher.h"
#include <Kismet/GameplayStatics.h>

void ULevelSwitcher::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("LevelSwitcherSubsystem - BeginPlay!!"));
}

void ULevelSwitcher::SwitchToLevel(const TSoftObjectPtr<UWorld> LevelAsset, const bool bUseServerTravel) const
{
	if (LevelAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LevelSwitcher - SwitchToLevel was called with null as Level asset"));
		return;
	}
	
	if (bUseServerTravel)
	{
		const FString URL = LevelAsset.LoadSynchronous()->GetMapName();// + TEXT("?game=/Game/Snake3D/Core/GameModes/Gameplay/GM_Gameplay.GM_Gameplay_C?listen");
		UE_LOG(LogTemp, Log, TEXT("%s"), *URL);
		if (GetWorld()->GetFirstPlayerController()->HasAuthority())
		{
			GetWorld()->ServerTravel(URL);
		}
		else
		{
			Server_TravelTo(URL);
		}
		
		return;
	}
		
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelAsset);
}

void ULevelSwitcher::Server_TravelTo_Implementation(const FString& URL) const
{
	GetWorld()->ServerTravel(URL);
}
