// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "SteamSession.generated.h"

UCLASS()
class SNAKE3D_API USteamSession : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	// --- Properties ---
private:
	IOnlineSessionPtr SessionInterface;
	FName DefaultSessionName;
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void Host(const bool bLan, TSoftObjectPtr<UWorld> Level) const;
	
	UFUNCTION(BlueprintCallable)
	void Join(const bool bLan) const;
	
	UFUNCTION(BlueprintCallable)
	void DestroySession() const;
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
	void OnFindSessionsComplete(bool bSuccess);
};
