// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SteamSession.generated.h"


UCLASS()
class SNAKE3D_API USteamSession : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	// --- Properties ---
private:
	IOnlineSessionPtr SessionInterface;
	FOnlineSessionSettings SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void Host(FName SessionName) const;
	
	UFUNCTION(BlueprintCallable)
	void GetAvailableSessions();
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
	void OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueNetId);
	void OnFindSessionsComplete(bool bSuccess);
	void OnRegisterPlayersComplete(FName SessionName, const TArray<TSharedRef<const FUniqueNetId>>& UniqueNetIds, bool SuccessFull);
};
