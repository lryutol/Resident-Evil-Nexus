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

    bDirectorControlsWaves = true;
    WaveSize = 20;
    DelayBetweenWaves = 3.0f;
    ExtraWaveSize = 10;

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
    bBossPhaseStarted = false;
    BossSpawnAttempts = 0;
    NextSpawnerIndex = -1;
    WaveDelayElapsed = 0.f;

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
    bBossPhaseStarted = false;
    BossSpawnAttempts = 0;
    WaveDelayElapsed = 0.f;
    // CellsTotal/CellsRemaining y ActiveEnemies NO se reinician: se registran en su propio BeginPlay

    // STRATEGY: estrategias de loot, protegidas del Garbage Collector
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

        // El estado de la Fachada sobrevive entre niveles: lo reiniciamos
        if (Fachada->GetGameState() != ENexusGameState::Exploring)
        {
            Fachada->SetGameState(ENexusGameState::Exploring);
        }
    }

    if (Spawners.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: no hay Spawners asignados en este nivel."));
    }

    if (bFinalBossLevel)
    {
        // Nivel final: el Tyrant esta desde el inicio y llegan oleadas periodicas
        GetWorldTimerManager().SetTimer(FinalBossTimer, this, &ANexusGameDirector::HandleBossPhase, 1.5f, false);
        GetWorldTimerManager().SetTimer(ExtraWaveTimer, this, &ANexusGameDirector::TriggerExtraWaves, FMath::Max(5.f, ExtraWaveInterval), true);
    }
    else if (bDirectorControlsWaves)
    {
        GetWorldTimerManager().SetTimer(WaveTimer, this, &ANexusGameDirector::EvaluateWaves, 0.5f, true, 1.0f);
    }

    if (bHuntEnabled)
    {
        GetWorldTimerManager().SetTimer(HuntTimer, this, &ANexusGameDirector::UpdateHunt, FMath::Max(0.2f, HuntUpdateInterval), true, 0.5f);
    }

    NotifyUIKillCount();
    NotifyUIScore();
}

void ANexusGameDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearAllTimersForObject(this);

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

    TimeRemaining -= DeltaTime;

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

    if (TimeRemaining <= 0.f)
    {
        TimeRemaining = 0.f;
        bLevelFinished = true;
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: se acabo el tiempo."));
        Fachada->SetGameState(ENexusGameState::GameOver);
        Fachada->ShowGameOver();
    }
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

// ---------- Registro de enemigos ----------
void ANexusGameDirector::RegisterEnemy(ANexusZombie* Enemy)
{
    if (Enemy) ActiveEnemies.AddUnique(Enemy);
}

void ANexusGameDirector::UnregisterEnemy(ANexusZombie* Enemy)
{
    if (Enemy) ActiveEnemies.Remove(Enemy);
}

int32 ANexusGameDirector::CountLiveEnemies() const
{
    int32 Live = 0;
    for (ANexusZombie* Candidate : ActiveEnemies)
    {
        if (IsValid(Candidate) && Candidate->GetZombieState() != EZombieState::Dead) Live++;
    }
    return Live;
}

// ---------- Caza: el Director guia a los enemigos hacia el jugador ----------
void ANexusGameDirector::UpdateHunt()
{
    if (bLevelFinished) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn) return;

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

    SurroundAngleOffset = FMath::Fmod(SurroundAngleOffset + SurroundSpinPerUpdate, 360.f);

    const FVector PlayerLocation = PlayerPawn->GetActorLocation();
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

    for (int32 Index = 0; Index < EnemyCount; ++Index)
    {
        ANexusZombie* Enemy = ActiveEnemies[Index];
        if (!IsValid(Enemy)) continue;

        // Si es el Tyrant y no esta activo (inconsciente, levantandose o rugiendo), limpiamos su punto de caza
        if (ANexusTyrantT1104* Tyrant = Cast<ANexusTyrantT1104>(Enemy))
        {
            if (!Tyrant->CanActAndMove())
            {
                Enemy->ClearHuntTarget();
                continue;
            }
        }

        // Reparto en circulo alrededor del jugador, en 3 anillos
        const float AngleRadians = FMath::DegreesToRadians(SurroundAngleOffset + (360.f / EnemyCount) * Index);
        const float RingRadius = SurroundRadius + 100.f * (Index % 3);

        FVector DesiredPoint = PlayerLocation + FVector(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians), 0.f) * RingRadius;
        FVector FinalTarget = PlayerLocation;

        if (NavSys)
        {
            FNavLocation NavPoint;
            if (NavSys->ProjectPointToNavigation(DesiredPoint, NavPoint, FVector(250.f, 250.f, 400.f)))
            {
                FinalTarget = NavPoint.Location;
            }
            else
            {
                FNavLocation FallbackPoint;
                if (NavSys->GetRandomPointInNavigableRadius(PlayerLocation, RingRadius, FallbackPoint))
                {
                    FinalTarget = FallbackPoint.Location;
                }
            }
        }

        Enemy->SetHuntTarget(FinalTarget);
    }
}

// ---------- Oleadas: solo sale la siguiente cuando TODO esta muerto ----------
bool ANexusGameDirector::AnySpawnerBusy() const
{
    for (ANexusZombieSpawner* S : Spawners)
    {
        if (S && S->IsSpawning()) return true;
    }
    return false;
}

ANexusZombieSpawner* ANexusGameDirector::PickNextSpawner()
{
    const int32 Count = Spawners.Num();
    for (int32 Step = 0; Step < Count; ++Step)
    {
        NextSpawnerIndex = (NextSpawnerIndex + 1) % Count;
        if (Spawners[NextSpawnerIndex]) return Spawners[NextSpawnerIndex];
    }
    return nullptr;
}

void ANexusGameDirector::EvaluateWaves()
{
    if (bLevelFinished || bFinalBossLevel || bBossPhaseStarted) return;

    if (AnySpawnerBusy())
    {
        WaveDelayElapsed = 0.f;
        return;
    }

    // Autocorreccion: si el contador dice que hay enemigos pero no queda ninguno vivo
    if (AliveEnemies > 0 && CountLiveEnemies() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: contador de enemigos desincronizado, se reinicia."));
        AliveEnemies = 0;
    }

    if (AliveEnemies > 0)
    {
        WaveDelayElapsed = 0.f;
        return;
    }

    // Cuota agotada: solo falta el jefe
    if (BossKillThreshold - KillCount - CountPendingMiniBosses() <= 0) return;

    WaveDelayElapsed += 0.5f;
    if (WaveDelayElapsed < DelayBetweenWaves) return;
    WaveDelayElapsed = 0.f;

    if (ANexusZombieSpawner* Chosen = PickNextSpawner())
    {
        UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: nueva oleada de %d en %s."), WaveSize, *Chosen->GetName());
        Chosen->StartWave(WaveSize);
    }
}

void ANexusGameDirector::TriggerExtraWaves()
{
    if (bLevelFinished) return;

    if (ANexusZombieSpawner* Chosen = PickNextSpawner())
    {
        Chosen->StartWave(ExtraWaveSize);
    }
}

// ---------- Cupos de enemigos ----------
int32 ANexusGameDirector::CountPendingMiniBosses() const
{
    int32 Pending = 0;
    if (MiniBossInterval > 0)
    {
        for (int32 Milestone = MiniBossInterval; Milestone < BossKillThreshold; Milestone += MiniBossInterval)
        {
            if (Milestone > KillCount) Pending++;
        }
    }
    return Pending;
}

bool ANexusGameDirector::TryRegisterSpawn()
{
    if (!bFinalBossLevel)
    {
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
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: celda destruida. Quedan %d."), CellsRemaining);
}

// ---------- Muertes, puntaje y jefes ----------
void ANexusGameDirector::NotifyEnemyKilled(ANexusZombie* DeadEnemy, bool bHeadshot)
{
    if (!DeadEnemy) return;

    const bool bIsBoss = Cast<ANexusTyrantT1104>(DeadEnemy) != nullptr;
    const bool bIsSpecial = Cast<ANexusZombieSpecial>(DeadEnemy) != nullptr;

    UnregisterEnemy(DeadEnemy);
    if (!bIsBoss) UnregisterSpawn();

    KillCount++;
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: enemigo eliminado. Total: %d"), KillCount);

    // Racha y puntaje
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

    // STRATEGY: loot por probabilidad
    const int32 Roll = FMath::RandRange(1, 100);
    const FVector DropLocation = DeadEnemy->GetActorLocation();
    UWorld* World = GetWorld();

    if (Roll <= 30) { PistolLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 50) { HerbLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 62) { ShotgunLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 72) { RifleLoot->GenerateLoot(World, DropLocation); }
    else if (Roll <= 77) { GrenadeLoot->GenerateLoot(World, DropLocation); }
    else { EmptyLoot->GenerateLoot(World, DropLocation); }

    // Jefes (solo en niveles normales)
    if (!bFinalBossLevel)
    {
        if (KillCount < BossKillThreshold && MiniBossInterval > 0 && (KillCount % MiniBossInterval) == 0)
        {
            HandleMiniBossPhase(KillCount / MiniBossInterval);
        }
        else if (KillCount >= BossKillThreshold && !bBossPhaseStarted)
        {
            HandleBossPhase();
        }
    }
}

void ANexusGameDirector::HandleMiniBossPhase(int32 Bracket)
{
    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: invocando Mini-Boss %d."), Bracket);

    if (ANexusZombieSpawner* Chosen = PickNextSpawner())
    {
        Chosen->SpawnMiniBoss(Bracket);
    }
}

void ANexusGameDirector::HandleBossPhase()
{
    if (bBossPhaseStarted) return;
    bBossPhaseStarted = true;

    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: INVOCANDO AL TYRANT T-1104."));

    // OBSERVER: el spawner marcado como jefe lo crea al recibir el estado
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->SetGameState(ENexusGameState::BossFight);
    }

    // Verificacion: si el Tyrant no aparece, el Director lo fuerza
    BossSpawnAttempts = 0;
    GetWorldTimerManager().SetTimer(BossVerifyTimer, this, &ANexusGameDirector::VerifyBossSpawned, 1.5f, false);
}

void ANexusGameDirector::VerifyBossSpawned()
{
    if (bLevelFinished) return;

    if (UGameplayStatics::GetActorOfClass(this, ANexusTyrantT1104::StaticClass())) return;

    BossSpawnAttempts++;
    if (BossSpawnAttempts > 3)
    {
        UE_LOG(LogTemp, Error, TEXT("DIRECTOR: el Tyrant no se pudo crear tras 3 intentos. Revisa spawners, NavMesh y BP_Tyrant."));
        return;
    }

    ANexusZombieSpawner* Chosen = nullptr;
    for (ANexusZombieSpawner* S : Spawners)
    {
        if (S && S->IsBossSpawner()) { Chosen = S; break; }
    }
    if (!Chosen)
    {
        for (ANexusZombieSpawner* S : Spawners)
        {
            if (S) { Chosen = S; break; }
        }
    }

    if (!Chosen)
    {
        UE_LOG(LogTemp, Error, TEXT("DIRECTOR: no hay ningun spawner para crear al Tyrant."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("DIRECTOR: el Tyrant no aparecio, forzando con %s (intento %d)."), *Chosen->GetName(), BossSpawnAttempts);
    Chosen->SpawnBoss();

    GetWorldTimerManager().SetTimer(BossVerifyTimer, this, &ANexusGameDirector::VerifyBossSpawned, 2.0f, false);
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