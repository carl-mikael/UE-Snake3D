// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelSwitcher.generated.h"

UCLASS()
class SNAKE3D_API ULevelSwitcher : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void SwitchToLevel(TSoftObjectPtr<UWorld> LevelAsset) const;
};
