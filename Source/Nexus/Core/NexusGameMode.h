#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexusGameMode.generated.h"

class UAudioComponent;
class USoundBase;

UCLASS(minimalapi)
class ANexusGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANexusGameMode();

	// Función que llamaremos cuando el jugador muere o termina el nivel
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayEndGameMusic(bool bIsVictory);

protected:
	virtual void BeginPlay() override;

	// Música del nivel
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* LevelMusic;

	// Música de Game Over
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* GameOverMusic;

	// Música de Victoria (Nivel Completado)
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* VictoryMusic;

	// Componente de audio para la música del nivel
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	UAudioComponent* MusicComponent;

private:
	bool bMusicStopped;
};