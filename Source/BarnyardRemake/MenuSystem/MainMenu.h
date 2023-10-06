// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuInterface.h"
#include "MenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	uint16 Ping;
	FString HostUsername;
};

UCLASS()
class BARNYARDREMAKE_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu();

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_Login;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_SinglePlayer;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_HostGame;

	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_JoinGame;

	/*
	UPROPERTY(Meta = (BindWidget))
	class UButton* JoinGameButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* HostGameButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton_SingleSetup;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton_Join;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton_Host;
	*/

	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_Options;

	UPROPERTY(Meta = (BindWidget))
	class UButton* btn_Quit;

	/*
	UPROPERTY(Meta = (BindWidget))
	class UButton* SinglePlayGameButton;

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* ServerList;
	*/

	UPROPERTY(Meta = (BindWidget))
	class UWidget* WBP_SinglePlayerScreen;
	
	UPROPERTY(Meta = (BindWidget))
	class UWidget* WBP_OptionsScreen;
	
	UPROPERTY(Meta = (BindWidget))
	class UWidget* WBP_JoinGameScreen;

	UPROPERTY(Meta = (BindWidget))
	class UWidget* WBP_HostGameScreen;

	/*
	UPROPERTY(Meta = (BindWidget))
	class UWidget* SinglePlayerSetupMenu;

	UPROPERTY(Meta = (BindWidget))
	class UEditableTextBox* SessionNameTextBox;
	*/

	UPROPERTY(Meta = (BindWidget))
	class UWidgetSwitcher* ContextMenu;

	/*
	UPROPERTY(Meta = (BindWidget))
	class UComboBoxString* HostMapCombo;
	
	UPROPERTY(Meta = (BindWidget))
    class UComboBoxString* HostGameModeCombo;
    
    UPROPERTY(Meta = (BindWidget))
    class UComboBoxString* SingleMapCombo;
    	
    UPROPERTY(Meta = (BindWidget))
    class UComboBoxString* SingleGameModeCombo;
    */
	
	TSubclassOf<UUserWidget> ServerRowClass;

	UFUNCTION()
	void Login();

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();
	
	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenOptionsMenu();

	UFUNCTION()
	void QuitPressed();

	UFUNCTION()
	void OpenSinglePlay();

	UFUNCTION()
	void StartSinglePlay();

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();

	TMap<FString, FString> Maps;
	TMap<FString, FString> GameModes;
};
