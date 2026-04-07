// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE3D_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:
	// --- Methods ---
	UFUNCTION(BlueprintCallable)
	void SwitchToLevel(TSoftObjectPtr<UWorld> LevelAsset) const;
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName SessionName) const;
};
