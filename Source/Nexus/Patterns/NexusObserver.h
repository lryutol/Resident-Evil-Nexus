#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NexusObserver.generated.h"

UENUM(BlueprintType)
enum class ENexusGameState : uint8
{
    Exploring,
    BossFight,
    GameOver,
    LevelComplete
};

UINTERFACE(MinimalAPI)
class UNexusObserver : public UInterface
{
    GENERATED_BODY()
};

class NEXUS_API INexusObserver
{
    GENERATED_BODY()

public:
    virtual void OnGameStateChanged(ENexusGameState NewState) = 0;
    virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) = 0;
    // El segundo valor es la municion de RESERVA
    virtual void OnAmmoChanged(int32 CurrentAmmo, int32 ReserveAmmo) = 0;
    virtual void OnEnemyCountChanged(int32 CurrentKills, int32 MaxKills) = 0;
    virtual void OnScoreChanged(int32 CurrentScore, int32 ComboCount) = 0;
    virtual void OnTimeBonus(float Seconds) = 0;
    virtual void OnCellsChanged(int32 CellsRemaining, int32 CellsTotal) = 0;
};