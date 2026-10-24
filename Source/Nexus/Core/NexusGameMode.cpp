// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusGameMode.h"
#include "../Characters/NexusCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANexusGameMode::ANexusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
