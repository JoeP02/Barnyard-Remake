// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BarnyardGameInstance.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class BARNYARDREMAKE_API SLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
