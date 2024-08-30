// Copyright Epic Games, Inc. All Rights Reserved.

#include "IDC_UP_GameProjectGameMode.h"
#include "IDC_UP_GameProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIDC_UP_GameProjectGameMode::AIDC_UP_GameProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
