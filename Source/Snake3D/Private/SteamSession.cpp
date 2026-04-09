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
	//const IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
	const IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get();
	if (!SteamSubsystem)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::Initialize() Online subsystem is null")
			);
		}
		
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
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true; // Allows Steam "Join Game"
	SessionSettings.bUsesPresence = true;         // Required for Steam Lobbies
	SessionSettings.bShouldAdvertise = true;      // Makes it searchable
	SessionSettings.bUseLobbiesIfAvailable = true; // Essential for Steam P2P
	SessionSettings.bAllowInvites = true;
	
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USteamSession::OnCreateSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USteamSession::OnJoinSessionComplete);
	SessionInterface->OnSessionParticipantJoinedDelegates.AddUObject(this, &USteamSession::OnSessionParticipantJoined);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USteamSession::OnFindSessionsComplete);
	SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(this, &USteamSession::OnRegisterPlayersComplete);
}

void USteamSession::Host(const FName SessionName) const
{
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::Host() LocalPlayer is null")
			);
		}
		
		return;
	}
	
	if (!SessionInterface.IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::Host() SessionInterface invalid")
			);
		}
		
		return;
	}
	
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SessionSettings);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::Host()")
		);
	}
	
	SessionInterface->GetNumSessions();
}

void USteamSession::GetAvailableSessions()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, SearchSettings.IsValid()? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::GetAvailableSessions() - Success: %s"), SearchSettings.IsValid()? TEXT("True") : TEXT("False"))
		);
	}
	
	if (!SearchSettings.IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::GetAvailableSessions() SearchSettings invalid")
			);
		}
		
		return;
	}
	SearchSettings->bIsLanQuery = true;
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

void USteamSession::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::OnJoinSessionComplete()!!!")
		);
	}

	const bool bRegisterSuccessful = SessionInterface->RegisterPlayer(SessionName, *GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, bRegisterSuccessful? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnJoinSessionComplete() Register Success: %hs"), bRegisterSuccessful? "True" : "False")
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
			-1, 5.f, bSuccess ? FColor::Green : FColor::Red,
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
			-1, 5.f, FColor::Red,
			TEXT("SteamSession::OnFindSessionsComplete() No sessions found"));
		
		return;
	}

	const bool bCouldJoin = SessionInterface->JoinSession(0, FName("TestSession"), SearchSettings->SearchResults[0]);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, bCouldJoin? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnFindSessionsComplete() - Success: %hs"), bCouldJoin? "True" : "False")
		);
	}
}

void USteamSession::OnRegisterPlayersComplete(FName SessionName,
	const TArray<TSharedRef<const FUniqueNetId>>& UniqueNetIds, bool SuccessFull)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, SuccessFull? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnRegisterPlayersComplete() - Success: %hs"), SuccessFull? "True" : "False")
		);
	}
}
