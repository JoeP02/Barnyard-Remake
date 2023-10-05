// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BarnyardGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BARNYARDREMAKE_API UBarnyardGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UBarnyardGameInstance();
	
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& LevelName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLevelLoaded);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush BackgroundImage;
	
	UFUNCTION(BlueprintCallable)
	AActor* GetDefaultActorObject(TSubclassOf<AActor> Actor);

	void Login();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void FindSession();
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	void OnFindSessionComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void GetAllFriends();
	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void ShowInviteUI();

	UFUNCTION(BlueprintCallable)
	void ShowFriendUI();

protected:
	class IOnlineSubsystem* OnlineSubsystem;

	bool bIsLoggedIn;
};
