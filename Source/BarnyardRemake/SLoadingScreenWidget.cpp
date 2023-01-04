// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreenWidget.h"

#include "LoadingScreenSettings.h"
#include "SlateOptMacros.h"
#include "BarnyardGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Images/SThrobber.h"
#include <iostream>
#include <time.h>

#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "Barnyard"

void SLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	const FSlateBrush* LoadingSlateBrush = &Settings->BackgroundImageSettings.BackgroundImage;
	const FSlateBrush* ThrobberSlateBrush = &Settings->ThrobberSettings.ThrobberImage;
	const FSlateFontInfo LoadingTextStyle = Settings->LoadingTextSettings.FontInfo;
	const FSlateColor LoadingTextColor = Settings->LoadingTextSettings.TextColor;
	const TArray<FText> LoadingTips = Settings->TipSettings.Tips;
	
	const FText LoadingText = LOCTEXT("LoadingText", "Loading");

	srand(time(NULL));
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image(LoadingSlateBrush)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::UserSpecified)
				.StretchDirection(EStretchDirection::Both)
				.UserSpecifiedScale(40.f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBackgroundBlur)
					.BlurStrength(10.f)
				]
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				// Horizontal Box
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(0.15f)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(FMargin(20.f, 0.f, 5.f, 20.f))
				[
					SNew(STextBlock)
					.Font(LoadingTextStyle)
					.ColorAndOpacity(LoadingTextColor)
					.Justification(ETextJustify::Left)
					.Text(LoadingText)
				]
				+ SHorizontalBox::Slot()
				.Padding(5.f, 0.f, 0.f, 20.f)
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Fill)
				.FillWidth(0.05f)
				[
					SNew(SThrobber)
					.PieceImage(ThrobberSlateBrush)
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(20.f, 0.f, 5.f, 20.f))
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Center)
				.FillWidth(1.f)
				[
					SNew(STextBlock)
					.Font(LoadingTextStyle)
					.ColorAndOpacity(LoadingTextColor)
					.Justification(ETextJustify::Center)
					.Text(LoadingTips[rand() % LoadingTips.Num()])
				]
				// HB End
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
