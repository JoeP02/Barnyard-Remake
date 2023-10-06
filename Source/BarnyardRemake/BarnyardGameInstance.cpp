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
#include "MenuSystem/MainMenu.h"
#include "Net/UnrealNetwork.h"

const FName DemoSessionName = FName("Test Session");
const static FName SESSION_NAME = TEXT("My Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UBarnyardGameInstance::UBarnyardGameInstance(const FObjectInitializer &ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/UI/MultiplayerMenu/WBP_MultiplayerMainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	
	MainMenu = MenuBPClass.Class;
	
	bIsLoggedIn = false;
}


void UBarnyardGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBarnyardGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBarnyardGameInstance::EndLoadingScreen);

	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnJoinSessionComplete);
	}

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UBarnyardGameInstance::OnNetworkFailure);
	}
	
	Login();
}

void UBarnyardGameInstance::LoadMenuWidget()
{
	if (!ensure(MainMenu != nullptr)) return;
	
	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenu);
	if (!ensure(MainMenuWidget != nullptr)) return;

	MainMenuWidget->Setup();	

	MainMenuWidget->SetMenuInterface(this);
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

void UBarnyardGameInstance::Host(FString ServerName, FString ServerAddress)
{
	UE_LOG(LogTemp, Warning, TEXT("Host Called Game Instance"));
	
	DesiredServerName = ServerName;
	DesiredServerAddress = ServerAddress;
	
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
		else
		{
			CreateSession();
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("JPG - Failed at Host()"));
}

void UBarnyardGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (!SessionSearch.IsValid())
	{
		return;
	}
	
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->Teardown();
	}
	
	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[Index]);
}

void UBarnyardGameInstance::SinglePlayer(FString MapName, FString GameMode)
{
}

void UBarnyardGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(NAME_GameSession);
	}
}

void UBarnyardGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		// SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("BarnyardLobby"), EOnlineComparisonOp::Equals);
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Session Find"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UBarnyardGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/MenuSystem/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
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
			if (Identity->GetLoginStatus(0) == ELoginStatus::Type::LoggedIn)
			{
				Identity->Logout(0);
			}
			
			FOnlineAccountCredentials Credentials;
			Credentials.Type = FString("developer");
			
			if (Credentials.Type == FString("developer"))
			{
				Credentials.Id = FString("localhost:8081");
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

void UBarnyardGameInstance::CreateSession()
{
	if (bIsLoggedIn)
	{
		if (SessionInterface.IsValid())
		{
			FOnlineSessionSettings SessionSettings;
			if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
			{
				SessionSettings.bIsLANMatch = true;
			}
			else
			{
				SessionSettings.bIsLANMatch = false;
			}
		
			SessionSettings.NumPublicConnections = 5;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.bIsDedicated = false;
			SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionSettings.Set(SEARCH_KEYWORDS, FString("BarnyardLobby"), EOnlineDataAdvertisementType::ViaOnlineService);
		
			SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);	
		}
	}
	else
	{
		ErrorScreenInstance = CreateWidget<UErrorMenu>(GetWorld(), ErrorScreen);
		ErrorScreenInstance->ErrorMessageToDisplay = FText::FromString("Failed To Create Session. User Not Logged In.");
		ErrorScreenInstance->AddToViewport();
	}
}

void UBarnyardGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ErrorScreenInstance = CreateWidget<UErrorMenu>(GetWorld(), ErrorScreen);
		ErrorScreenInstance->ErrorMessageToDisplay = FText::FromString("Failed To Create Session. Please Check Your Network Connection.");
		ErrorScreenInstance->AddToViewport();
		return;
	}
	
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->Teardown();
	}
	
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/Levels/Lobby?listen");

	UE_LOG(LogTemp, Warning, TEXT("JPG - End Of OnCreateSessionComplete"));
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
	if (bWasSuccessful)
	{
		CreateSession();
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
				
				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UBarnyardGameInstance::OnFindSessionsComplete);
				SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
			}
		}
	}
}

void UBarnyardGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid() && MainMenuWidget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Session Find"));

		TArray<FServerData> ServerNames;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;
			Data.Name = *SearchResult.GetSessionIdStr();
			Data.HostUsername = *SearchResult.Session.OwningUserName;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.Ping = SearchResult.PingInMs;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				Data.Name = "Name Unavailable";
			}
			
			ServerNames.Add(Data);
		}

		MainMenuWidget->SetServerList(ServerNames);
	}
	else
	{
		ErrorScreenInstance = CreateWidget<UErrorMenu>(GetWorld(), ErrorScreen);
		ErrorScreenInstance->ErrorMessageToDisplay = FText::FromString("Failed To Join Session. Please Check Your Network Connection.");
		ErrorScreenInstance->AddToViewport();
	}
}

void UBarnyardGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}
	
	FString Address;
	if(!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could Not Get Connect String"));
		return;
	}
	
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
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

void UBarnyardGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	LoadMainMenu();
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

