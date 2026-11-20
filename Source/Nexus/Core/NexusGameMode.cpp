// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusGameMode.h"
#include "NexusSubsystem.h" // Incluimos la Fachada
#include "../Patterns/NexusObserver.h" // <-- INCLUIMOS EL ENUM
#include "../Characters/NexusCharacter.h" 
#include "UObject/ConstructorHelpers.h"

ANexusGameMode::ANexusGameMode()
{
	// Buscamos tu Blueprint del personaje
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/BP_NexusCharacter"));

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		DefaultPawnClass = ANexusCharacter::StaticClass();
	}
}

void ANexusGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 2. USAMOS LA FACHADA (FACADE PATTERN)
	if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
	{
		// CORREGIDO: Usamos el Enum ENexusGameState en lugar de TEXT("Explorando")
		Fachada->SetGameState(ENexusGameState::Exploring);
	}
}