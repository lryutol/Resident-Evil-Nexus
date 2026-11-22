#include "NexusGameDirector.h"
#include "NexusSubsystem.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "../AI/NexusZombieSpawner.h"
#include "../Characters/NexusZombie.h"
#include "../Characters/NexusZombieSpecial.h"
#include "../Characters/NexusTyrantT1104.h"
#include "../Patterns/ConcreteLootStrategies.h"

ANexusGameDirector::ANexusGameDirector()
{
    PrimaryActorTick.bCanEverTick = true;

    bFinalBossLevel = false;
    BossKillThreshold = 59;
    MiniBossInterval = 20;
    InitialTime = 120.f;
    HeadshotBonusTime = 10.f;
    ExtraWaveInterval = 120.f;

    CommonPoints = 100;
    SpecialPoints = 500;
    BossPoints = 2000;
    HeadshotBonusPoints = 50;
    ComboWindow = 6.f;

    bHuntEnabled = true;
    HuntUpdateInterval = 1.0f;
    SurroundRadius = 600.f;
    SurroundSpinPerUpdate = 20.f;
    SurroundAngleOffset = 0.f;

    KillCount = 0;
    Score = 0;
    ComboCount = 0;
    ComboTimer = 0.f;
    TimeRemaining = InitialTime;
    AliveEnemies = 0;
    CellsTotal = 0;
    CellsRemaining = 0;
    bLevelFinished = false;

    PistolLoot = nullptr;
    RifleLoot = nullptr;
    ShotgunLoot = nullptr;
    HerbLoot = nullptr;
    GrenadeLoot = nullptr;
    EmptyLoot = nullptr;
}

void ANexusGameDirector::BeginPlay()
{
    Super::BeginPlay();

    KillCount = 0;
    Score = 0;
    ComboCount = 0;
    AliveEnemies = 0;
    TimeRemaining = InitialTime;
    bLevelFinished = false;
    // CellsTotal/CellsRemaining NO se reinician: las celdas se registran en su propio BeginPlay
    // ActiveEnemies NO se reinicia: los enemigos colocados en el mapa se registran en su BeginPlay

    // Estrategias de loot (Patron Strategy), guardadas en un UPROPERTY para el GC
    UPistolLootStrategy* Pistol = NewObject<UPistolLootStrategy>(this);
    URifleLootStrategy* Rifle = NewObject<URifleLootStrategy>(this);
    UShotgunLootStrategy* Shotgun = NewObject<UShotgunLootStrategy>(this);
    UHerbLootStrategy* Herb = NewObject<UHerbLootStrategy>(this);
    UGrenadeLootStrategy* Grenade = NewObject<UGrenadeLootStrategy>(this);
    UEmptyLootStrategy* Empty = NewObject<UEmptyLootStrategy>(this);

    PistolLoot = Pistol;
    RifleLoot = Rifle;
    ShotgunLoot = Shotgun;
    HerbLoot = Herb;
    GrenadeLoot = Grenade;
    EmptyLoot = Empty;

    LootStrategyRefs.Add(Pistol);
    LootStrategyRefs.Add(Rifle);
    LootStrategyRefs.Add(Shotgun);
    LootStrategyRefs.Add(Herb);
    LootStrategyRefs.Add(Grenade);
    LootStrategyRefs.Add(Empty);

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->AttachObserver(this);
    }

    if (Spawners.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: No hay Spawners asignados en este nivel."));
    }

    // Nivel final: el Tyrant esta desde el inicio y llegan oleadas periodicas
    if (bFinalBossLevel)
    {
        GetWorldTimerManager().SetTimer(FinalBossTimer, this, &ANexusGameDirector::HandleBossPhase, 1.5f, false);
        GetWorldTimerManager().SetTimer(ExtraWaveTimer, this, &ANexusGameDirector::TriggerExtraWaves, ExtraWaveInterval, true);
    }

    // Caza: el Director guia a los enemigos hacia el jugador
    if (bHuntEnabled)
    {
        GetWorldTimerManager().SetTimer(HuntTimer, this, &ANexusGameDirector::UpdateHunt, FMath::Max(0.2f, HuntUpdateInterval), true, 0.5f);
    }

    NotifyUIKillCount();
    NotifyUIScore();
}

void ANexusGameDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(HuntTimer);

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->DetachObserver(this);
    }
    Super::EndPlay(EndPlayReason);
}

void ANexusGameDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bLevelFinished) return;

    UNexusSubsystem* Fachada = UNexusSubsystem::Get(this);
    if (!Fachada) return;

    const ENexusGameState State = Fachada->GetGameState();
    if (State == ENexusGameState::GameOver || State == ENexusGameState::LevelComplete) return;

    // Reloj del nivel
    TimeRemaining -= DeltaTime;

    // Tiempo de racha
    if (ComboCount > 0)
    {
        ComboTimer -= DeltaTime;
        if (ComboTimer <= 0.f)
        {
            ComboTimer = 0.f;
            ComboCount = 0;
            NotifyUIScore();
        }
    }

    // Se acabo el tiempo: fin de la partida
    if (TimeRemaining <= 0.f)
    {
        TimeRemaining = 0.f;
        bLevelFinished = true;
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: Se acabo el tiempo."));
        Fachada->SetGameState(ENexusGameState::GameOver);
        Fachada->ShowGameOver();
    }
}

void ANexusGameDirector::OnGameStateChanged(ENexusGameState NewState)
{
}

void ANexusGameDirector::AttachUI(INexusObserver* UI)
{
    if (UI && !UIObservers.Contains(UI)) UIObservers.Add(UI);
}

void ANexusGameDirector::DetachUI(INexusObserver* UI)
{
    if (UI) UIObservers.Remove(UI);
}

float ANexusGameDirector::GetComboRatio() const
{
    if (ComboCount <= 0 || ComboWindow <= 0.f) return 0.f;
    return FMath::Clamp(ComboTimer / ComboWindow, 0.f, 1.f);
}

// 1.0 con racha 1, sube 0.1 por cada muerte extra, tope 3.0
float ANexusGameDirector::GetComboMultiplier() const
{
    return FMath::Min(3.0f, 1.0f + 0.1f * FMath::Max(0, ComboCount - 1));
}

void ANexusGameDirector::NotifyUIKillCount()
{
    for (INexusObserver* UI : UIObservers)
    {
        if (UI) UI->OnEnemyCountChanged(KillCount, GetKillGoal());
    }
}

void ANexusGameDirector::NotifyUIScore()
{
    for (INexusObserver* UI : UIObservers)
    {
        if (UI) UI->OnScoreChanged(Score, ComboCount);
    }
}

void ANexusGameDirector::NotifyUICells()
{
    for (INexusObserver* UI : UIObservers)
    {
        if (UI) UI->OnCellsChanged(CellsRemaining, CellsTotal);
    }
}

void ANexusGameDirector::AddBonusTime(float Seconds)
{
    if (bLevelFinished || Seconds <= 0.f) return;

    TimeRemaining += Seconds;
    for (INexusObserver* UI : UIObservers)
    {
        if (UI) UI->OnTimeBonus(Seconds);
    }
}

// ---------- Caza de enemigos ----------

void ANexusGameDirector::RegisterEnemy(ANexusZombie* Enemy)
{
    if (Enemy)
    {
        ActiveEnemies.AddUnique(Enemy);
    }
}

void ANexusGameDirector::UnregisterEnemy(ANexusZombie* Enemy)
{
    if (Enemy)
    {
        ActiveEnemies.Remove(Enemy);
    }
}

void ANexusGameDirector::UpdateHunt()
{
    if (bLevelFinished) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn) return;

    // Limpiamos enemigos destruidos o muertos
    for (int32 i = ActiveEnemies.Num() - 1; i >= 0; --i)
    {
        ANexusZombie* Candidate = ActiveEnemies[i];
        if (!IsValid(Candidate) || Candidate->GetZombieState() == EZombieState::Dead)
        {
            ActiveEnemies.RemoveAt(i);
        }
    }

    const int32 EnemyCount = ActiveEnemies.Num();
    if (EnemyCount == 0) return;

    // El circulo gira un poco en cada actualizacion
    SurroundAngleOffset = FMath::Fmod(SurroundAngleOffset + SurroundSpinPerUpdate, 360.f);

    const FVector PlayerLocation = PlayerPawn->GetActorLocation();
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

    for (int32 Index = 0; Index < EnemyCount; ++Index)
    {
        // Reparto en circulo alrededor del jugador, en 3 anillos de radio
        const float AngleDegrees = SurroundAngleOffset + (360.f / EnemyCount) * Index;
        const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
        const float RingRadius = SurroundRadius + 100.f * (Index % 3);

        FVector TargetPoint = PlayerLocation + FVector(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians), 0.f) * RingRadius;

        if (NavSys)
        {
            FNavLocation NavPoint;
            if (NavSys->ProjectPointToNavigation(TargetPoint, NavPoint, FVector(300.f, 300.f, 500.f)))
            {
                TargetPoint = NavPoint.Location;
            }
            else
            {
                // Sin NavMesh en ese punto: va directo hacia el jugador
                TargetPoint = PlayerLocation;
            }
        }

        ActiveEnemies[Index]->SetHuntTarget(TargetPoint);
    }
}

// ---------- Cupos de enemigos ----------

int32 ANexusGameDirector::CountPendingMiniBosses() const
{
    int32 Pending = 0;
    if (MiniBossInterval > 0)
    {
        for (int32 M = MiniBossInterval; M < BossKillThreshold; M += MiniBossInterval)
        {
            if (M > KillCount) Pending++;
        }
    }
    return Pending;
}

bool ANexusGameDirector::TryRegisterSpawn()
{
    if (!bFinalBossLevel)
    {
        // Total de enemigos antes del jefe = BossKillThreshold, reservando los mini bosses
        const int32 Allowed = BossKillThreshold - KillCount - AliveEnemies - CountPendingMiniBosses();
        if (Allowed <= 0) return false;
    }

    AliveEnemies++;
    return true;
}

void ANexusGameDirector::UnregisterSpawn()
{
    AliveEnemies = FMath::Max(0, AliveEnemies - 1);
}

void ANexusGameDirector::RegisterAlive()
{
    AliveEnemies++;
}

// ---------- Celdas ----------

void ANexusGameDirector::RegisterCell()
{
    CellsTotal++;
    CellsRemaining++;
    NotifyUICells();
}

void ANexusGameDirector::NotifyCellDestroyed()
{
    CellsRemaining = FMath::Max(0, CellsRemaining - 1);
    NotifyUICells();
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: Celda destruida. Quedan %d."), CellsRemaining);
}

// ---------- Muertes, puntaje y jefes ----------

void ANexusGameDirector::NotifyEnemyKilled(ANexusZombie* DeadEnemy, bool bHeadshot)
{
    if (!DeadEnemy) return;

    const bool bIsBoss = Cast<ANexusTyrantT1104>(DeadEnemy) != nullptr;
    const bool bIsSpecial = Cast<ANexusZombieSpecial>(DeadEnemy) != nullptr;

    UnregisterEnemy(DeadEnemy);

    // El Tyrant no se registra en la cuota
    if (!bIsBoss) UnregisterSpawn();

    KillCount++;
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: Enemigo eliminado. Total: %d"), KillCount);

    // 1. Racha y puntaje
    ComboCount++;
    ComboTimer = ComboWindow;

    int32 Points = bIsBoss ? BossPoints : (bIsSpecial ? SpecialPoints : CommonPoints);
    if (bHeadshot)
    {
        Points += HeadshotBonusPoints;
        AddBonusTime(HeadshotBonusTime);
    }
    Score += FMath::RoundToInt(Points * GetComboMultiplier());

    NotifyUIKillCount();
    NotifyUIScore();

    // 2. Patron Strategy: loot por probabilidad
    const int32 Roll = FMath::RandRange(1, 100);
    const FVector DropLocation = DeadEnemy->GetActorLocation();
    UWorld* World = GetWorld();

    if (Roll <= 30) { PistolLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 50) { HerbLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 62) { ShotgunLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 72) { RifleLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 77) { GrenadeLoot->GenerateLoot(World, DropLocation); }
    else { EmptyLoot->GenerateLoot(World, DropLocation); }

    // 3. Jefes (solo en niveles normales)
    if (!bFinalBossLevel)
    {
        if (KillCount < BossKillThreshold && MiniBossInterval > 0 && (KillCount % MiniBossInterval) == 0)
        {
            HandleMiniBossPhase(KillCount / MiniBossInterval);
        }
        else if (KillCount == BossKillThreshold)
        {
            HandleBossPhase();
        }
    }
}

void ANexusGameDirector::HandleMiniBossPhase(int32 Bracket)
{
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: Invocando Mini-Boss %d!"), Bracket);

    TArray<ANexusZombieSpawner*> Valid;
    for (ANexusZombieSpawner* S : Spawners)
    {
        if (S) Valid.Add(S);
    }

    if (Valid.Num() > 0)
    {
        Valid[FMath::RandRange(0, Valid.Num() - 1)]->SpawnMiniBoss(Bracket);
    }
}

void ANexusGameDirector::HandleBossPhase()
{
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: INVOCANDO AL TYRANT T-1104 (Builder)!"));

    // Observer: el spawner marcado como "Boss Spawner" lo crea al recibir el estado
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->SetGameState(ENexusGameState::BossFight);
    }
}

void ANexusGameDirector::TriggerExtraWaves()
{
    if (bLevelFinished) return;

    for (ANexusZombieSpawner* S : Spawners)
    {
        if (S) S->TriggerWave();
    }
}

void ANexusGameDirector::NotifyBossDefeated()
{
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: Tyrant derrotado. Escenario completado."));

    bLevelFinished = true;

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->SetGameState(ENexusGameState::LevelComplete);
    }

    GetWorldTimerManager().SetTimer(BossDefeatedTimer, this, &ANexusGameDirector::FinishScenario, 4.0f, false);
}

void ANexusGameDirector::FinishScenario()
{
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->OnScenarioCompleted();
    }
}