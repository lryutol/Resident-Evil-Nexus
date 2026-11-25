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

    virtual void OnGameStateChanged(ENexusGameState NewState) override;
    virtual void OnHealthChanged(float, float) override {}
    virtual void OnAmmoChanged(int32, int32) override {}
    virtual void OnEnemyCountChanged(int32, int32) override {}
    virtual void OnScoreChanged(int32, int32) override {}
    virtual void OnTimeBonus(float) override {}
    virtual void OnCellsChanged(int32, int32) override {}

    void StartWave(int32 Count);
    void SpawnMiniBoss(int32 Bracket);
    bool SpawnBoss();

    bool IsSpawning() const { return PendingToSpawn > 0; }
    bool IsBossSpawner() const { return bIsBossSpawner; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "Spawner") TSubclassOf<UNexusEnemyFactory> EnemyFactoryClass;
    UPROPERTY(EditAnywhere, Category = "Spawner") TArray<ANexusSpawnZone*> SpawnZones;
    UPROPERTY(EditAnywhere, Category = "Spawner") bool bIsBossSpawner;
    UPROPERTY(EditAnywhere, Category = "Spawner") float SpawnInterval;

    UPROPERTY() UNexusEnemyFactory* CurrentFactory;
    UPROPERTY() ANexusGameDirector* Director;

    int32 PendingToSpawn;
    bool bBossSpawned;
    FTimerHandle SpawnTimerHandle;

    bool EnsureFactory();
    bool FindSpawnLocation(FVector& OutLocation) const;
    void SpawnNext();
};