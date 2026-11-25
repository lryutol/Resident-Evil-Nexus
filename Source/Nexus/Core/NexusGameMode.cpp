#include "NexusGameMode.h"
#include "NexusSubsystem.h"
#include "NexusSettingsSave.h"
#include "../Patterns/NexusObserver.h"
#include "../Characters/NexusCharacter.h" 
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

ANexusGameMode::ANexusGameMode()
{
	// ASIGNACIÓN DEL PERSONAJE CON LA RUTA CORREGIDA
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_NexusCharacter"));

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		// Fallback por si acaso
		DefaultPawnClass = ANexusCharacter::StaticClass();
	}

	bMusicStopped = false;
	MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	MusicComponent->bAutoActivate = false;
}

void ANexusGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Ocultar avisos de pantalla del motor (iluminacion, streaming pool, etc.)
	if (GEngine)
	{
		GEngine->Exec(GetWorld(), TEXT("DisableAllScreenMessages"));
	}

	UNexusSubsystem* Fachada = UNexusSubsystem::Get(this);
	if (Fachada)
	{
		Fachada->HideLoadingScreen();
		Fachada->SetGameState(ENexusGameState::Exploring);
	}

	// Pedimos el volumen a la Fachada
	float MusicVolume = 1.0f;
	if (Fachada && Fachada->GetSettings())
	{
		MusicVolume = Fachada->GetSettings()->MusicVolume;
	}

	if (LevelMusic && MusicComponent && MusicVolume > 0.0f)
	{
		MusicComponent->SetSound(LevelMusic);
		MusicComponent->bIsUISound = true;
		MusicComponent->FadeIn(2.0f, MusicVolume, 0.0f);
	}
}

void ANexusGameMode::PlayEndGameMusic(bool bIsVictory)
{
	if (bMusicStopped || !MusicComponent) return;
	bMusicStopped = true;

	MusicComponent->FadeOut(1.5f, 0.0f);

	USoundBase* EndMusic = bIsVictory ? VictoryMusic : GameOverMusic;
	if (EndMusic)
	{
		FTimerHandle MusicTimer;
		FTimerDelegate MusicDelegate;
		MusicDelegate.BindLambda([this, EndMusic]() {
			float FinalVolume = 1.0f;
			if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
			{
				if (Fachada->GetSettings()) FinalVolume = Fachada->GetSettings()->MusicVolume;
			}
			UGameplayStatics::PlaySound2D(this, EndMusic, FinalVolume);
			});
		GetWorldTimerManager().SetTimer(MusicTimer, MusicDelegate, 1.5f, false);
	}
}