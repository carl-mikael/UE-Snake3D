// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamSession.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void USteamSession::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Log, TEXT("SteamSession::Initialize() - BeginPlay!!"));
	
	// ReSharper disable once CppBoundToDelegateMethodIsNotMarkedAsUFunction
	const IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
	if (!SteamSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSession::Initialize() - SteamSubsystem is null"));
		return;
	}

	SessionInterface = SteamSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSession::Initialize() - SessionInterface is null"));
		return;
	}
	SearchSettings = MakeShared<FOnlineSessionSearch>();
	
	SessionSettings = FOnlineSessionSettings();
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true; // Allows Steam "Join Game"
	SessionSettings.bUsesPresence = true;         // Required for Steam Lobbies
	SessionSettings.bShouldAdvertise = true;      // Makes it searchable
	SessionSettings.bUseLobbiesIfAvailable = true; // Essential for Steam P2P
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
	
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USteamSession::OnCreateSessionComplete);
	SessionInterface->OnSessionParticipantJoinedDelegates.AddUObject(this, &USteamSession::OnSessionParticipantJoined);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USteamSession::OnFindSessionsComplete);
}

void USteamSession::Host(const FName SessionName) const
{
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSession::HostSession() - LocalPlayer is null"));
		return;
	}
	
	if (!SessionInterface.IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::Initialize() SessionInterface invalid")
			);
		}
		
		return;
	}
	
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SessionSettings);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::HostSession()")
		);
	}
	
	SessionInterface->GetNumSessions();
}

void USteamSession::GetAvailableSessions()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::GetAvailableSessions() - Success: " + SearchSettings.IsValid() ? "True" : "False")
		);
	}
	
	if (!SearchSettings.IsValid())
	{
		return;
	}
	SearchSettings->bIsLanQuery = false;
	SessionInterface->FindSessions(0, SearchSettings.ToSharedRef());
}

void USteamSession::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::OnCreateSessionComplete()")
		);
	}
}

void USteamSession::OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueNetId)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::OnSessionParticipantJoined()")
		);
	}
}

void USteamSession::OnFindSessionsComplete(bool bSuccess)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			FString::Printf(TEXT("SteamSession::OnFindSessionsComplete() - Success: %hs"), bSuccess? "True" : "False")
		);
		
		if (!bSuccess)
		{
			return;
		}
	}
	
	const bool bHasSearchResult = SearchSettings->SearchResults.Num() > 0;
	if (!bHasSearchResult)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::OnFindSessionsComplete() No sessions found"));
		return;
	}

	const bool bCouldJoin = SessionInterface->JoinSession(0, FName("TestSession"), SearchSettings->SearchResults[0]);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			FString::Printf(TEXT("SteamSession::OnFindSessionsComplete() - Success: %hs"), bCouldJoin? "True" : "False")
		);
	}
}
