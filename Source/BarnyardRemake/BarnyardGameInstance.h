// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BarnyardGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BARNYARDREMAKE_API UBarnyardGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& LevelName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLevelLoaded);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush BackgroundImage;
};
