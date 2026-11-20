#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Patterns/NexusObserver.h"
#include "NexusSpawnZone.h"
#include "NexusZombieSpawner.generated.h"

class UNexusEnemyFactory;
class ANexusGameDirector;

UCLASS()
class NEXUS_API ANexusZombieSpawner : public AActor, public INexusObserver
{
    GENERATED_BODY()

public:
    ANexusZombieSpawner();

    // Observer
    virtual void OnGameStateChanged(ENexusGameState NewState) override;
    virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) override {}
    virtual void OnAmmoChanged(int32 CurrentAmmo, int32 ReserveAmmo) override {}
    virtual void OnEnemyCountChanged(int32 CurrentKills, int32 MaxKills) override {}
    virtual void OnScoreChanged(int32 CurrentScore, int32 ComboCount) override {}
    virtual void OnTimeBonus(float Seconds) override {}
    virtual void OnCellsChanged(int32 CellsRemaining, int32 CellsTotal) override {}

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void SpawnMiniBoss(int32 Bracket);

    // Inicia una oleada a pedido (nivel final)
    void TriggerWave();

    // Los zombies de este spawner avisan aqui al morir
    void NotifyEnemyDied();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "Spawner") TSubclassOf<UNexusEnemyFactory> EnemyFactoryClass;
    UPROPERTY(EditAnywhere, Category = "Spawner") TArray<ANexusSpawnZone*> SpawnZones;
    UPROPERTY(EditAnywhere, Category = "Spawner") TSubclassOf<class ANexusTyrantT1104> BossClass;

    // Si es true, este spawner crea al Tyrant (solo uno por nivel)
    UPROPERTY(EditAnywhere, Category = "Spawner") bool bIsBossSpawner;

    // Si es false, solo spawnea cuando el Director llama a TriggerWave
    UPROPERTY(EditAnywhere, Category = "Waves") bool bAutoStartWaves;

    UPROPERTY(EditAnywhere, Category = "Waves") int32 EnemiesPerWave;
    UPROPERTY(EditAnywhere, Category = "Waves") float SpawnInterval;
    UPROPERTY(EditAnywhere, Category = "Waves") float DelayAfterWaveCleared;

    int32 EnemiesSpawnedThisWave;
    int32 SpawnedOkThisWave;
    int32 AliveInWave;
    bool bWaveActive;
    bool bSpawning;
    bool bLastWaveEmpty;
    bool bBossSpawned;

    FTimerHandle WaveTimerHandle;
    FTimerHandle SpawnTimerHandle;

    // UPROPERTY evita que el Garbage Collector destruya estos objetos
    UPROPERTY()
    UNexusEnemyFactory* CurrentFactory;

    UPROPERTY()
    ANexusGameDirector* Director;

    void StartWave();
    void SpawnZombie();
    void FinishSpawning();
    void OnWaveCleared();
    void SpawnTyrant();
};