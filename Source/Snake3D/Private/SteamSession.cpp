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
	const IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (!OnlineSubSystem)
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
	
	SessionInterface = OnlineSubSystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSession::Initialize() - SessionInterface is null"));
		return;
	}
	SearchSettings = MakeShared<FOnlineSessionSearch>();
	
	DebugSessionName = TEXT("TestSession");
	
	SessionSettings = FOnlineSessionSettings();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings.bAntiCheatProtected = false;
	SessionSettings.bUsesStats = false;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
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

void USteamSession::Host() const
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
	
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), DebugSessionName, SessionSettings);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::Host() CreateSession requested")
		);
	}
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
			-1, 5.f, bSuccess ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnCreateSessionComplete() Success=%s"), bSuccess ? TEXT("True") : TEXT("False"))
		);
	}

	if (!bSuccess || !SessionInterface.IsValid())
	{
		return;
	}

	const bool bStartSuccess = SessionInterface->StartSession(SessionName);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, bStartSuccess ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::StartSession() Success=%s"), bStartSuccess ? TEXT("True") : TEXT("False"))
		);
	}
}

void USteamSession::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	constexpr int32 MaxJoinRetries = 5;
	constexpr int32 MaxJoinResolveRetries = 10;
	static int32 JoinRetryCount = 0;
	static int32 JoinResolveRetryCount = 0;

	UWorld* World = GetWorld();
	auto ShowScreen = [](const FString& Msg, const FColor Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 6.f, Color, Msg);
		}
	};

	auto JoinResultToText = [](const EOnJoinSessionCompleteResult::Type Result)
	{
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::Success:
			return TEXT("Success");
		case EOnJoinSessionCompleteResult::SessionIsFull:
			return TEXT("SessionIsFull");
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			return TEXT("SessionDoesNotExist");
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			return TEXT("CouldNotRetrieveAddress");
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			return TEXT("AlreadyInSession");
		default:
			return TEXT("Unknown");
		}
	};

	if (!SessionInterface.IsValid())
	{
		ShowScreen(TEXT("Join complete: SessionInterface invalid"), FColor::Red);
		return;
	}

	auto* NetDriver = World ? World->GetNetDriver() : nullptr;
	ShowScreen(
		FString::Printf(TEXT("Join complete. Result=%s(%d) Session=%s NetDriver=%s"),
			JoinResultToText(JoinResult),
			static_cast<int32>(JoinResult),
			*SessionName.ToString(),
			NetDriver ? *NetDriver->GetClass()->GetName() : TEXT("null")),
		JoinResult == EOnJoinSessionCompleteResult::Success ? FColor::Green : FColor::Yellow
	);

	if (JoinResult != EOnJoinSessionCompleteResult::Success)
	{
		if (JoinRetryCount < MaxJoinRetries && SearchSettings.IsValid() && SearchSettings->SearchResults.Num() > 0)
		{
			const ULocalPlayer* LocalPlayer = World ? World->GetFirstLocalPlayerFromController() : nullptr;
			if (!IsValid(LocalPlayer) || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
			{
				ShowScreen(TEXT("Join retry failed: LocalPlayer or NetId invalid"), FColor::Red);
				return;
			}

			++JoinRetryCount;
			const bool bRetryIssued = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SearchSettings->SearchResults[0]);
			ShowScreen(
				FString::Printf(TEXT("Join failed (%s). Retry %d/%d issued=%s"),
					JoinResultToText(JoinResult),
					JoinRetryCount,
					MaxJoinRetries,
					bRetryIssued ? TEXT("True") : TEXT("False")),
				bRetryIssued ? FColor::Yellow : FColor::Red
			);
			return;
		}

		JoinRetryCount = 0;
		ShowScreen(FString::Printf(TEXT("Join failed after retries (%s)"), JoinResultToText(JoinResult)), FColor::Red);
		return;
	}

	JoinRetryCount = 0;

	FString ConnectInfo;
	bool bResolvedConnectInfo = SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo);
	if ((!bResolvedConnectInfo || ConnectInfo.IsEmpty()) && SearchSettings.IsValid() && SearchSettings->SearchResults.Num() > 0)
	{
		bResolvedConnectInfo = SessionInterface->GetResolvedConnectString(SearchSettings->SearchResults[0], NAME_GamePort, ConnectInfo);
	}

	if (!bResolvedConnectInfo || ConnectInfo.IsEmpty())
	{
		if (JoinResolveRetryCount < MaxJoinResolveRetries && SearchSettings.IsValid() && SearchSettings->SearchResults.Num() > 0)
		{
			const ULocalPlayer* LocalPlayer = World ? World->GetFirstLocalPlayerFromController() : nullptr;
			if (!IsValid(LocalPlayer) || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
			{
				ShowScreen(TEXT("Resolve retry failed: LocalPlayer or NetId invalid"), FColor::Red);
				return;
			}

			++JoinResolveRetryCount;
			const bool bRetryIssued = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SearchSettings->SearchResults[0]);
			ShowScreen(
				FString::Printf(TEXT("Connect info unresolved. Retry %d/%d (issued=%s)"),
					JoinResolveRetryCount,
					MaxJoinResolveRetries,
					bRetryIssued ? TEXT("True") : TEXT("False")),
				bRetryIssued ? FColor::Yellow : FColor::Red
			);
			return;
		}

		JoinResolveRetryCount = 0;
		ShowScreen(TEXT("Could not resolve connect info after retries"), FColor::Red);
		return;
	}

	JoinResolveRetryCount = 0;

	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!IsValid(PlayerController))
	{
		ShowScreen(TEXT("Join complete: PlayerController invalid"), FColor::Red);
		return;
	}

	ShowScreen(FString::Printf(TEXT("ClientTravel -> %s"), *ConnectInfo), FColor::Green);
	PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
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

	ULocalPlayer* LocalPlayer = GetWorld() ? GetWorld()->GetFirstLocalPlayerFromController() : nullptr;
	if (!IsValid(LocalPlayer) || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SteamSession::OnFindSessionsComplete() LocalPlayer/NetId invalid"));
		}
		return;
	}

	const bool bCouldJoin = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), DebugSessionName, SearchSettings->SearchResults[0]);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, bCouldJoin? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnFindSessionsComplete() Could join session: %hs"), bCouldJoin? "True" : "False")
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
