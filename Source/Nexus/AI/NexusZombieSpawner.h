#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusEnemyTypes.h"
#include "NexusZombieSpawner.generated.h"

class UNexusZombieFactory;

UCLASS()
class NEXUS_API ANexusZombieSpawner : public AActor
{
    GENERATED_BODY()

public:
    ANexusZombieSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    UNexusZombieFactory* ZombieFactory;

    // --- CONFIGURACIÓN DE OLEADAS ---
    UPROPERTY(EditAnywhere, Category = "Waves")
    int32 EnemiesPerWave;

    UPROPERTY(EditAnywhere, Category = "Waves")
    float TimeBetweenWaves;

    UPROPERTY(EditAnywhere, Category = "Waves")
    float SpawnInterval;

    UPROPERTY(EditAnywhere, Category = "Waves")
    float SpawnRadius;

    // Variables de control interno
    int32 CurrentWave;
    int32 EnemiesSpawnedThisWave;

    FTimerHandle WaveTimerHandle;
    FTimerHandle SpawnTimerHandle;

    void StartWave();
    void SpawnZombie();
    void OnWaveFinished();
};