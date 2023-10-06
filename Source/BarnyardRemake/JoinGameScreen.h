// Jameson Peirson Games 2023

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MenuWidget.h"
#include "JoinGameScreen.generated.h"

/**
 * 
 */
UCLASS()
class BARNYARDREMAKE_API UJoinGameScreen : public UMenuWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* SessionList;
	
};
