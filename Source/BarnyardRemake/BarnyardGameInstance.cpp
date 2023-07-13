// Fill out your copyright notice in the Description page of Project Settings.


#include "BarnyardGameInstance.h"

#include "SLoadingScreenWidget.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "BarnyardRemake/SLoadingScreenWidget.h"

void UBarnyardGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBarnyardGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBarnyardGameInstance::EndLoadingScreen);
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

