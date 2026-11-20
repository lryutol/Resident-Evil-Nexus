#include "NexusZombieSpawner.h" // SIEMPRE EL PRIMERO
#include "NexusEnemyFactory.h"
#include "../Characters/NexusZombie.h" 
#include "../Characters/NexusTyrantT1104.h"
#include "../Patterns/NexusTyrantBuilder.h"
#include "../Core/NexusSubsystem.h"
#include "../Core/NexusGameDirector.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

ANexusZombieSpawner::ANexusZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    bIsBossSpawner = false;
    bAutoStartWaves = true;
    EnemiesPerWave = 10;
    SpawnInterval = 1.0f;
    DelayAfterWaveCleared = 3.0f;

    EnemiesSpawnedThisWave = 0;
    SpawnedOkThisWave = 0;
    AliveInWave = 0;
    bWaveActive = false;
    bSpawning = false;
    bLastWaveEmpty = false;
    bBossSpawned = false;

    CurrentFactory = nullptr;
    Director = nullptr;
}

void ANexusZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    Director = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(this, ANexusGameDirector::StaticClass()));

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->AttachObserver(this);

        // Mas dificil = menos pausa entre oleadas
        const float Rate = FMath::Max(0.1f, Fachada->GetDifficultyParams().SpawnRateMultiplier);
        DelayAfterWaveCleared = DelayAfterWaveCleared / Rate;
    }

    if (EnemyFactoryClass) CurrentFactory = NewObject<UNexusEnemyFactory>(this, EnemyFactoryClass);

    if (bAutoStartWaves && CurrentFactory)
    {
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ANexusZombieSpawner::StartWave, 2.0f, false);
    }
}

void ANexusZombieSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this)) Fachada->DetachObserver(this);
    Super::EndPlay(EndPlayReason);
}

void ANexusZombieSpawner::OnGameStateChanged(ENexusGameState NewState)
{
    if (NewState == ENexusGameState::BossFight && bIsBossSpawner && !bBossSpawned)
    {
        SpawnTyrant();
        bBossSpawned = true;
    }
}

void ANexusZombieSpawner::SpawnTyrant()
{
    if (!BossClass) return;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLocation = GetActorLocation();
    if (SpawnZones.Num() > 0 && SpawnZones[0])
    {
        SpawnLocation = SpawnZones[0]->GetRandomPointInZone();
    }

    ANexusTyrantT1104* Tyrant = GetWorld()->SpawnActor<ANexusTyrantT1104>(BossClass, SpawnLocation, FRotator::ZeroRotator, Params);

    if (Tyrant)
    {
        UNexusTyrantBuilder* Builder = NewObject<UNexusTyrantBuilder>(this);
        Builder->BuildBaseStats(Tyrant);
        Builder->BuildArmor(Tyrant);
        Builder->BuildClaws(Tyrant);
        UE_LOG(LogTemp, Warning, TEXT("SPAWNER: El Tyrant T-1104 ha sido ensamblado y spawneado!"));
    }
}

void ANexusZombieSpawner::SpawnMiniBoss(int32 Bracket)
{
    if (!CurrentFactory || SpawnZones.Num() == 0) return;

    ANexusSpawnZone* Zone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];
    if (!Zone) return;

    ANexusZombie* MiniBoss = CurrentFactory->CreateEnemy(GetWorld(), Zone->GetRandomPointInZone(), FRotator::ZeroRotator, EZombieType::Special);
    if (MiniBoss && Director)
    {
        Director->RegisterAlive();
    }
    UE_LOG(LogTemp, Warning, TEXT("SPAWNER: Mini-Boss %d spawneado!"), Bracket);
}

void ANexusZombieSpawner::TriggerWave()
{
    if (!bWaveActive)
    {
        StartWave();
    }
}

void ANexusZombieSpawner::StartWave()
{
    if (!CurrentFactory || SpawnZones.Num() == 0) return;

    bWaveActive = true;
    bSpawning = true;
    EnemiesSpawnedThisWave = 0;
    SpawnedOkThisWave = 0;

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ANexusZombieSpawner::SpawnZombie, SpawnInterval, true, 0.0f);
}

void ANexusZombieSpawner::SpawnZombie()
{
    // El Director controla la cuota total de enemigos del nivel
    if (Director && !Director->TryRegisterSpawn())
    {
        FinishSpawning();
        return;
    }

    ANexusSpawnZone* Zone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];
    FVector SpawnLocation = Zone ? Zone->GetRandomPointInZone() : GetActorLocation();

    FNavLocation NavLocation;
    if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        if (NavSys->ProjectPointToNavigation(SpawnLocation, NavLocation, FVector(100, 100, 500)))
        {
            SpawnLocation = NavLocation.Location;
        }
    }

    ANexusZombie* SpawnedEnemy = CurrentFactory->CreateEnemy(GetWorld(), SpawnLocation, FRotator::ZeroRotator, EZombieType::Common);
    EnemiesSpawnedThisWave++;

    if (SpawnedEnemy)
    {
        SpawnedEnemy->SetOriginSpawner(this);
        SpawnedOkThisWave++;
        AliveInWave++;
    }
    else if (Director)
    {
        Director->UnregisterSpawn(); // fallo: devolvemos el cupo
    }

    if (EnemiesSpawnedThisWave >= EnemiesPerWave)
    {
        FinishSpawning();
    }
}

void ANexusZombieSpawner::FinishSpawning()
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    bSpawning = false;
    bLastWaveEmpty = (SpawnedOkThisWave == 0);

    if (AliveInWave <= 0)
    {
        OnWaveCleared();
    }
}

void ANexusZombieSpawner::NotifyEnemyDied()
{
    AliveInWave = FMath::Max(0, AliveInWave - 1);

    if (bWaveActive && !bSpawning && AliveInWave == 0)
    {
        OnWaveCleared();
    }
}

void ANexusZombieSpawner::OnWaveCleared()
{
    bWaveActive = false;
    UE_LOG(LogTemp, Warning, TEXT("SPAWNER: Oleada eliminada."));

    // Si la ultima oleada no pudo spawnear nada, la cuota se agoto: no hay mas
    if (bLastWaveEmpty) return;

    if (bAutoStartWaves)
    {
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ANexusZombieSpawner::StartWave, DelayAfterWaveCleared, false);
    }
}