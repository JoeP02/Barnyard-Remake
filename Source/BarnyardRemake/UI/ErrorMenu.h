// Jameson Peirson Games 2023

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ErrorMenu.generated.h"

/**
 * 
 */
UCLASS()
class BARNYARDREMAKE_API UErrorMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FText ErrorMessageToDisplay;
	
};
