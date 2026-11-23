#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Patterns/NexusObserver.h"
#include "NexusGameDirector.generated.h"

class ANexusZombie;
class ANexusZombieSpawner;
class INexusLootStrategy;

UCLASS()
class NEXUS_API ANexusGameDirector : public AActor, public INexusObserver
{
    GENERATED_BODY()

public:
    ANexusGameDirector();
    virtual void Tick(float DeltaTime) override;

    // OBSERVER
    virtual void OnGameStateChanged(ENexusGameState) override {}
    virtual void OnHealthChanged(float, float) override {}
    virtual void OnAmmoChanged(int32, int32) override {}
    virtual void OnEnemyCountChanged(int32, int32) override {}
    virtual void OnScoreChanged(int32, int32) override {}
    virtual void OnTimeBonus(float) override {}
    virtual void OnCellsChanged(int32, int32) override {}

    UFUNCTION(BlueprintCallable, Category = "Director") void NotifyEnemyKilled(ANexusZombie* DeadEnemy, bool bHeadshot);
    UFUNCTION(BlueprintCallable, Category = "Director") void NotifyBossDefeated();
    UFUNCTION(BlueprintCallable, Category = "Director") void AddBonusTime(float Seconds);

    // Celdas de energia
    void RegisterCell();
    void NotifyCellDestroyed();

    // Cupos de enemigos (los spawners piden permiso)
    bool TryRegisterSpawn();
    void UnregisterSpawn();
    void RegisterAlive();

    // Registro de enemigos vivos (para guiarlos hacia el jugador)
    void RegisterEnemy(ANexusZombie* Enemy);
    void UnregisterEnemy(ANexusZombie* Enemy);

    // La UI se suscribe aqui
    void AttachUI(INexusObserver* UI);
    void DetachUI(INexusObserver* UI);

    // Datos para el HUD
    int32 GetScore() const { return Score; }
    int32 GetKillCount() const { return KillCount; }
    int32 GetComboCount() const { return ComboCount; }
    float GetTimeRemaining() const { return TimeRemaining; }
    float GetComboRatio() const;
    float GetComboMultiplier() const;
    int32 GetCellsRemaining() const { return CellsRemaining; }
    int32 GetCellsTotal() const { return CellsTotal; }
    int32 GetKillGoal() const { return bFinalBossLevel ? 0 : BossKillThreshold + 1; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ---------- CONFIGURACION POR NIVEL ----------
    UPROPERTY(EditInstanceOnly, Category = "Director|Level") TArray<ANexusZombieSpawner*> Spawners;
    UPROPERTY(EditAnywhere, Category = "Director|Level") bool bFinalBossLevel;
    UPROPERTY(EditAnywhere, Category = "Director|Level") int32 BossKillThreshold;
    UPROPERTY(EditAnywhere, Category = "Director|Level") int32 MiniBossInterval;

    UPROPERTY(EditAnywhere, Category = "Director|Time") float InitialTime;
    UPROPERTY(EditAnywhere, Category = "Director|Time") float HeadshotBonusTime;
    UPROPERTY(EditAnywhere, Category = "Director|Time") float ExtraWaveInterval;

    // ---------- OLEADAS (el Director las controla) ----------
    UPROPERTY(EditAnywhere, Category = "Director|Waves") bool bDirectorControlsWaves;
    UPROPERTY(EditAnywhere, Category = "Director|Waves") int32 WaveSize;
    UPROPERTY(EditAnywhere, Category = "Director|Waves") float DelayBetweenWaves;
    UPROPERTY(EditAnywhere, Category = "Director|Waves") int32 ExtraWaveSize;

    // ---------- PUNTAJE ----------
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 CommonPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 SpecialPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 BossPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 HeadshotBonusPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") float ComboWindow;

    // ---------- CAZA ----------
    UPROPERTY(EditAnywhere, Category = "Director|Hunt") bool bHuntEnabled;
    UPROPERTY(EditAnywhere, Category = "Director|Hunt") float HuntUpdateInterval;
    UPROPERTY(EditAnywhere, Category = "Director|Hunt") float SurroundRadius;
    UPROPERTY(EditAnywhere, Category = "Director|Hunt") float SurroundSpinPerUpdate;

    // ---------- ESTADO ----------
    UPROPERTY(VisibleAnywhere, Category = "Director") int32 KillCount;

    UPROPERTY() TArray<ANexusZombie*> ActiveEnemies;

    int32 Score;
    int32 ComboCount;
    float ComboTimer;
    float TimeRemaining;
    int32 AliveEnemies;
    int32 CellsTotal;
    int32 CellsRemaining;
    bool bLevelFinished;
    bool bBossPhaseStarted;
    int32 BossSpawnAttempts;
    int32 NextSpawnerIndex;
    float WaveDelayElapsed;
    float SurroundAngleOffset;

    // STRATEGY: loot
    INexusLootStrategy* PistolLoot;
    INexusLootStrategy* RifleLoot;
    INexusLootStrategy* ShotgunLoot;
    INexusLootStrategy* HerbLoot;
    INexusLootStrategy* GrenadeLoot;
    INexusLootStrategy* EmptyLoot;

    UPROPERTY() TArray<UObject*> LootStrategyRefs;

    TArray<INexusObserver*> UIObservers;

    FTimerHandle BossDefeatedTimer;
    FTimerHandle FinalBossTimer;
    FTimerHandle ExtraWaveTimer;
    FTimerHandle HuntTimer;
    FTimerHandle WaveTimer;
    FTimerHandle BossVerifyTimer;

    void NotifyUIKillCount();
    void NotifyUIScore();
    void NotifyUICells();
    int32 CountPendingMiniBosses() const;
    int32 CountLiveEnemies() const;
    bool AnySpawnerBusy() const;
    ANexusZombieSpawner* PickNextSpawner();

    void EvaluateWaves();
    void UpdateHunt();
    void HandleMiniBossPhase(int32 Bracket);
    void HandleBossPhase();
    void VerifyBossSpawned();
    void TriggerExtraWaves();
    void FinishScenario();
};