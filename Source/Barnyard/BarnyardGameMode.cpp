// Copyright Epic Games, Inc. All Rights Reserved.

#include "BarnyardGameMode.h"
#include "BarnyardCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABarnyardGameMode::ABarnyardGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
