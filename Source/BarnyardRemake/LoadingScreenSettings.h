// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LoadingScreenSettings.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FThrobberSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush ThrobberImage;
};

USTRUCT(BlueprintType)
struct FLoadingTextSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateFontInfo FontInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateColor TextColor;
};

USTRUCT(BlueprintType)
struct FBackgroundImageSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush BackgroundImage;
};

USTRUCT(BlueprintType)
struct FTipSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FText> Tips;
};

UCLASS(Config="Game", defaultconfig, meta=(DisplayName="Barnyard - Loading Screen"))
class BARNYARDREMAKE_API ULoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	FBackgroundImageSettings BackgroundImageSettings;

	UPROPERTY(Config, EditAnywhere)
	FThrobberSettings ThrobberSettings;

	UPROPERTY(Config, EditAnywhere)
	FLoadingTextSettings LoadingTextSettings;
	
	UPROPERTY(Config, EditAnywhere)
	FTipSettings TipSettings;
};
