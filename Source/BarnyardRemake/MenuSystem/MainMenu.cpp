// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

UMainMenu::UMainMenu()
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/UI/MultiplayerMenu/WBP_SessionResult"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	Maps.Add(TEXT("Map 1"), TEXT("Map-1"));
	Maps.Add(TEXT("Map 2"), TEXT("Map-2"));

	GameModes.Add(TEXT("Kill Em All"), TEXT("KEA"));

	if (!ensure(btn_Login != nullptr)) return false;
	btn_Login->OnClicked.AddDynamic(this, &UMainMenu::Login);
	
	if (!ensure(btn_SinglePlayer != nullptr)) return false;
	btn_SinglePlayer->OnClicked.AddDynamic(this, &UMainMenu::OpenSinglePlay);
	
	if (!ensure(btn_HostGame != nullptr)) return false;
	btn_HostGame->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(btn_JoinGame != nullptr)) return false;
	btn_JoinGame->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	/*
	if (!ensure(BackButton_SingleSetup != nullptr)) return false;
	BackButton_SingleSetup->OnClicked.AddDynamic(this, &UMainMenu::OpenOptionsMenu);
	
	if (!ensure(BackButton_Join != nullptr)) return false;
	BackButton_Join->OnClicked.AddDynamic(this, &UMainMenu::OpenOptionsMenu);

	if (!ensure(BackButton_Host != nullptr)) return false;
	BackButton_Host->OnClicked.AddDynamic(this, &UMainMenu::OpenOptionsMenu);
	
	if (!ensure(JoinGameButton != nullptr)) return false;
	JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(HostGameButton != nullptr)) return false;
	HostGameButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	*/

	if (!ensure(btn_Options != nullptr)) return false;
	btn_Options->OnClicked.AddDynamic(this, &UMainMenu::OpenOptionsMenu);
	
	if (!ensure(btn_Quit != nullptr)) return false;
	btn_Quit->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	/*
	if (!ensure(SinglePlayGameButton != nullptr)) return false;
	SinglePlayGameButton->OnClicked.AddDynamic(this, &UMainMenu::StartSinglePlay);
	*/
	
	return true;
}

void UMainMenu::Login()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Login();
	}
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		// FString* Map = Maps.Find("null");
		// FString* GameMode = GameModes.Find("null");
		
		FString ServerName = "Session Name";
		MenuInterface->Host(ServerName, "/Game/Levels/Barnyard?Listen");
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Menu Interface is nullptr"));
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ContextMenu->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->txt_ServerName->SetText(FText::FromString(ServerData.Name));
		Row->txt_ServerHost->SetText(FText::FromString(ServerData.HostUsername));
		Row->txt_PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
		Row->txt_Ping->SetText(FText::FromString(FString::Printf(TEXT("%dms"), ServerData.Ping)));
		Row->Setup(this, i);
		++i;
		
		ContextMenu->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ContextMenu->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ContextMenu->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index: %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index Not Set"));
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(ContextMenu != nullptr)) return;
	if (!ensure(WBP_JoinGameScreen != nullptr)) return;
	ContextMenu->SetActiveWidget(WBP_JoinGameScreen);
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenHostMenu()
{
	if (!ensure(ContextMenu != nullptr)) return;
	if (!ensure(WBP_HostGameScreen != nullptr)) return;
	ContextMenu->SetActiveWidget(WBP_HostGameScreen);

	HostServer();
}

void UMainMenu::OpenOptionsMenu()
{
	if (!ensure(ContextMenu != nullptr)) return;
	if (!ensure(WBP_OptionsScreen != nullptr)) return;
	ContextMenu->SetActiveWidget(WBP_OptionsScreen);
}

void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}

void UMainMenu::OpenSinglePlay()
{
	if (!ensure(ContextMenu != nullptr)) return;
	if (!ensure(WBP_SinglePlayerScreen != nullptr)) return;
	ContextMenu->SetActiveWidget(WBP_SinglePlayerScreen);
}

void UMainMenu::StartSinglePlay()
{
	if (MenuInterface != nullptr)
	{
		FString* Map = Maps.Find("null");
		FString* GameMode = GameModes.Find("null");
		try
		{
			MenuInterface->SinglePlayer(*Map, *GameMode);
		}
		catch (...)
		{
			UE_LOG(LogTemp, Warning, TEXT("Map: %s or GameMode: %s does not exist!"), Map, GameMode);
		}
	}
}

