// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamSession.h"

#include "Kismet/GameplayStatics.h"

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
	
	SessionSettings = FOnlineSessionSettings();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings.bAntiCheatProtected = false;
	SessionSettings.bUsesStats = false;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	SessionSettings.NumPublicConnections = 100;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bAllowInvites = true;
	
	SearchSettings = MakeShared<FOnlineSessionSearch>();
	
	DefaultSessionName = TEXT("DefaultSession");
	
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USteamSession::OnCreateSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USteamSession::OnJoinSessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USteamSession::OnFindSessionsComplete);
}

void USteamSession::Host(const bool bLan, TSoftObjectPtr<UWorld> Level) const
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
	
	SearchSettings->bIsLanQuery = bLan;
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), DefaultSessionName, SessionSettings);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Green,
			TEXT("SteamSession::Host() CreateSession requested")
		);
	}
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), Level, true, TEXT("listen"));
}

void USteamSession::Join(const bool bLan) const
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
				TEXT("SteamSession::GetAvailableSessions() DefaultSearchSettings invalid")
			);
		}
		
		return;
	}
	
	SearchSettings->bIsLanQuery = bLan;
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

	const bool bStartSuccess = SessionInterface->StartSession(DefaultSessionName);
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
	FString ConnectInfo;
	const bool bResolvedConnectInfo = SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo);
	if (!bResolvedConnectInfo)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::OnJoinSessionComplete() - Couldn't resolve connect info")
			);
		}
		
		return;
	}
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("SteamSession::OnJoinSessionComplete() - PlayerController is invalid")
			);
		}
		
		return;
	}

	PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
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
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer) || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SteamSession::OnFindSessionsComplete() LocalPlayer/NetId invalid"));
		}
		return;
	}
	
	const FOnlineSessionSearchResult& FirstResult = SearchSettings->SearchResults[0];
	const bool bCouldJoin = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), DefaultSessionName, FirstResult);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, bCouldJoin? FColor::Green : FColor::Red,
			FString::Printf(TEXT("SteamSession::OnFindSessionsComplete() Could join session: %hs"), bCouldJoin? "True" : "False")
		);
	}
}