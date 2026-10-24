#include "NexusZombieSpawner.h"
#include "NexusZombieFactory.h"
#include "../Characters/NexusZombie.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

ANexusZombieSpawner::ANexusZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // Valores por defecto
    SpawnInterval = 2.0f;
    SpawnRadius = 500.0f;
    EnemiesPerWave = 5;
    TimeBetweenWaves = 10.0f;
    ZombieFactory = nullptr;

    CurrentWave = 0;
    EnemiesSpawnedThisWave = 0;
}

void ANexusZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (ZombieFactory)
    {
        // Iniciamos la primera oleada después de 2 segundos
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ANexusZombieSpawner::StartWave, 2.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NEXUS SPAWNER: ¡Falta asignar la ZombieFactory!"));
    }
}

void ANexusZombieSpawner::StartWave()
{
    CurrentWave++;
    EnemiesSpawnedThisWave = 0;

    UE_LOG(LogTemp, Warning, TEXT("NEXUS SPAWNER: ¡Iniciando Oleada %d! Spawneando %d enemigos."), CurrentWave, EnemiesPerWave);

    // Empezamos a spawnear zombies uno por uno cada 'SpawnInterval' segundos
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ANexusZombieSpawner::SpawnZombie, SpawnInterval, true, 0.0f);
}

void ANexusZombieSpawner::SpawnZombie()
{
    if (!ZombieFactory) return;

    EZombieType TypeToSpawn = EZombieType::Common;
    TSubclassOf<ANexusZombie> ZombieClass = ZombieFactory->GetZombieClass(TypeToSpawn);

    if (ZombieClass)
    {
        FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.f, SpawnRadius);
        FVector SpawnLocation = GetActorLocation() + RandomOffset;

        // Buscamos el suelo usando el NavMesh para evitar que floten
        FNavLocation NavLocation;
        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavSys && NavSys->ProjectPointToNavigation(SpawnLocation, NavLocation, FVector(100, 100, 500)))
        {
            SpawnLocation = NavLocation.Location;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        GetWorld()->SpawnActor<ANexusZombie>(ZombieClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

        EnemiesSpawnedThisWave++;
        UE_LOG(LogTemp, Warning, TEXT("NEXUS SPAWNER: Zombie %d/%d spawneado."), EnemiesSpawnedThisWave, EnemiesPerWave);

        // Si ya hemos spawneado todos los de esta oleada, detenemos el spawner y esperamos a la siguiente
        if (EnemiesSpawnedThisWave >= EnemiesPerWave)
        {
            GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
            OnWaveFinished();
        }
    }
}

void ANexusZombieSpawner::OnWaveFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("NEXUS SPAWNER: Oleada %d terminada. Esperando %f segundos para la siguiente..."), CurrentWave, TimeBetweenWaves);

    // Esperamos el tiempo configurado y volvemos a empezar
    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ANexusZombieSpawner::StartWave, TimeBetweenWaves, false);
}