// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class BARNYARDREMAKE_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	
private:
	UPROPERTY(Meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void CancelPressed();

	UFUNCTION()
	void QuitPressed();
	
};
