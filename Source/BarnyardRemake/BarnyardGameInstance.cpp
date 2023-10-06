// Fill out your copyright notice in the Description page of Project Settings.


#include "BarnyardGameInstance.h"

#include "OnlineSubsystem.h"
#include "SLoadingScreenWidget.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "BarnyardRemake/SLoadingScreenWidget.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

const FName DemoSessionName = FName("Test Session");

UBarnyardGameInstance::UBarnyardGameInstance()
{
	bIsLoggedIn = false;
}


void UBarnyardGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBarnyardGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBarnyardGameInstance::EndLoadingScreen);

	OnlineSubsystem = IOnlineSubsystem::Get();
	Login();
}

void UBarnyardGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBarnyardGameInstance, numberOfPlayers);
}

void UBarnyardGameInstance::BeginLoadingScreen(const FString& LevelName)
{
	// UE_LOG(LogTemp, Warning, TEXT("Loading Level: %s"), *LevelName);
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	// LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreenWidget);

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UBarnyardGameInstance::EndLoadingScreen(UWorld* InLoadedLevel)
{

}

AActor* UBarnyardGameInstance::GetDefaultActorObject(TSubclassOf<AActor> Actor)
{
	return Actor.GetDefaultObject();
}

void UBarnyardGameInstance::Login()
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Type = FString("developer");
			
			if (Credentials.Type == FString("developer"))
			{
				Credentials.Id = FString("localhost:8080");
				Credentials.Token = FString("JosephPeirson");
			}
			else if (Credentials.Type == FString("accountportal"))
			{
				Credentials.Id = FString();
				Credentials.Token = FString();
			}
			
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UBarnyardGameInstance::OnLoginComplete);
			Identity->Login(0, Credentials);
		}
	}
}

void UBarnyardGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Logged In: %d"), bWasSuccessful);
	bIsLoggedIn = bWasSuccessful;
	
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void UBarnyardGameInstance::CreateSession(int32 NumberOfPlayers)
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = false;
				SessionSettings.bIsLANMatch = false;
				SessionSettings.bShouldAdvertise = true;
				SessionSettings.NumPublicConnections = NumberOfPlayers;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;
				SessionSettings.bUseLobbiesIfAvailable = true;

				numberOfPlayers = NumberOfPlayers;
				
				SessionSettings.Set(SEARCH_KEYWORDS, FString("UpskillLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnCreateSessionComplete);
				SessionPtr->CreateSession(0, DemoSessionName, SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Session Cannot Be Created: User Not Logged In"));
	}
}

void UBarnyardGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful)

	if (OnlineSubsystem && bWasSuccessful)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);

			UGameplayStatics::OpenLevel(GetWorld(), "Lobby", true, FString("listen"));
		}
	}
	else
	{
		ErrorScreenInstance = CreateWidget<UErrorMenu>(GetWorld(), ErrorScreen);
		ErrorScreenInstance->ErrorMessageToDisplay = FText::FromString("Failed To Create Session. Please Check Your Network Connection.");
		ErrorScreenInstance->AddToViewport();
	}
}

void UBarnyardGameInstance::DestroySession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(DemoSessionName);
			}
		}
	}
}

void UBarnyardGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful)

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}
}

void UBarnyardGameInstance::FindSession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SearchSettings = MakeShareable(new FOnlineSessionSearch());
				SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("UpskillLobby"), EOnlineComparisonOp::Equals);
				SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
				
				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnFindSessionComplete);
				SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
			}
		}
	}
}

void UBarnyardGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d Lobbies"), SearchSettings->SearchResults.Num());
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				if (SearchSettings->SearchResults.Num())
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnJoinSessionComplete);
					SessionPtr->JoinSession(0, DemoSessionName, SearchSettings->SearchResults[0]);
				}
			}
		}
	}
	else
	{
		ErrorScreenInstance = CreateWidget<UErrorMenu>(GetWorld(), ErrorScreen);
		ErrorScreenInstance->ErrorMessageToDisplay = FText::FromString("Failed To Join Session. Please Check Your Network Connection.");
		ErrorScreenInstance->AddToViewport();
	}
	
	if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
	{
		SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
	}
}

void UBarnyardGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			FString ConnectionInfo = FString();
			SessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);
			if (!ConnectionInfo.IsEmpty())
			{
				if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PC->ClientTravel(ConnectionInfo, TRAVEL_Absolute);
				}
			}
		}
	}
}

void UBarnyardGameInstance::GetAllFriends()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UBarnyardGameInstance::OnReadFriendsListComplete));
			}
		}
	}
}

void UBarnyardGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName,
	const FString& Error)
{
	if (bWasSuccessful)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> FriendsList;
				if (FriendsPtr->GetFriendsList(0, ListName, FriendsList))
				{
					for (TSharedRef<FOnlineFriend> Friend : FriendsList)
					{
						FString FriendName = Friend.Get().GetDisplayName();
						UE_LOG(LogTemp, Warning, TEXT("Friend: %s"), *FriendName);
					}
				}
			}
		}
	}
}

void UBarnyardGameInstance::ShowInviteUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
			{
				UIPtr->ShowInviteUI(0, DemoSessionName);
			}
		}
	}
}

void UBarnyardGameInstance::ShowFriendUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
			{
				UIPtr->ShowFriendsUI(0);
			}
		}
	}
}

