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

    // Observer: recibe avisos de la Fachada
    virtual void OnGameStateChanged(ENexusGameState NewState) override;
    virtual void OnHealthChanged(float InCurrentHealth, float InMaxHealth) override {}
    virtual void OnAmmoChanged(int32 InCurrentAmmo, int32 InReserveAmmo) override {}
    virtual void OnEnemyCountChanged(int32 InCurrentKills, int32 InMaxKills) override {}
    virtual void OnScoreChanged(int32 InScore, int32 InCombo) override {}
    virtual void OnTimeBonus(float InSeconds) override {}
    virtual void OnCellsChanged(int32 InCellsRemaining, int32 InCellsTotal) override {}

    // El enemigo llama a esto al morir
    UFUNCTION(BlueprintCallable, Category = "Director")
    void NotifyEnemyKilled(ANexusZombie* DeadEnemy, bool bHeadshot);

    // El Tyrant llama a esto al morir: termina el escenario
    UFUNCTION(BlueprintCallable, Category = "Director")
    void NotifyBossDefeated();

    // Suma tiempo al reloj (relojes de arena y tiros a la cabeza)
    UFUNCTION(BlueprintCallable, Category = "Director")
    void AddBonusTime(float Seconds);

    // Celdas de energia (nivel final)
    void RegisterCell();
    void NotifyCellDestroyed();

    // Control de cupos de enemigos (los spawners piden permiso)
    bool TryRegisterSpawn();
    void UnregisterSpawn();
    void RegisterAlive();

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
    // Todos los spawners del nivel (el Director los coordina)
    UPROPERTY(EditInstanceOnly, Category = "Director|Level")
    TArray<ANexusZombieSpawner*> Spawners;

    // True solo en Laboratorios: el Tyrant esta desde el inicio y hay celdas
    UPROPERTY(EditAnywhere, Category = "Director|Level")
    bool bFinalBossLevel;

    // Con esta cantidad de muertes aparece el Tyrant (59: el Tyrant es el 60)
    UPROPERTY(EditAnywhere, Category = "Director|Level")
    int32 BossKillThreshold;

    // Cada cuantas muertes aparece un mini boss
    UPROPERTY(EditAnywhere, Category = "Director|Level")
    int32 MiniBossInterval;

    // Tiempo inicial del nivel (segundos)
    UPROPERTY(EditAnywhere, Category = "Director|Time")
    float InitialTime;

    // Segundos que suma matar de un tiro a la cabeza
    UPROPERTY(EditAnywhere, Category = "Director|Time")
    float HeadshotBonusTime;

    // Cada cuantos segundos aparece una oleada extra (nivel final)
    UPROPERTY(EditAnywhere, Category = "Director|Time")
    float ExtraWaveInterval;

    // ---------- PUNTAJE (estilo Mercenarios) ----------
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 CommonPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 SpecialPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 BossPoints;
    UPROPERTY(EditAnywhere, Category = "Director|Score") int32 HeadshotBonusPoints;

    // Segundos que dura la racha sin matar antes de perderse
    UPROPERTY(EditAnywhere, Category = "Director|Score") float ComboWindow;

    // ---------- ESTADO ----------
    UPROPERTY(VisibleAnywhere, Category = "Director")
    int32 KillCount;

    int32 Score;
    int32 ComboCount;
    float ComboTimer;
    float TimeRemaining;
    int32 AliveEnemies;
    int32 CellsTotal;
    int32 CellsRemaining;
    bool bLevelFinished;

    // Estrategias de Loot (Patron Strategy)
    INexusLootStrategy* PistolLoot;
    INexusLootStrategy* RifleLoot;
    INexusLootStrategy* ShotgunLoot;
    INexusLootStrategy* HerbLoot;
    INexusLootStrategy* GrenadeLoot;
    INexusLootStrategy* EmptyLoot;

    // Mantiene vivas las estrategias (evita que el Garbage Collector las destruya)
    UPROPERTY()
    TArray<UObject*> LootStrategyRefs;

    TArray<INexusObserver*> UIObservers;

    FTimerHandle BossDefeatedTimer;
    FTimerHandle FinalBossTimer;
    FTimerHandle ExtraWaveTimer;

    void NotifyUIKillCount();
    void NotifyUIScore();
    void NotifyUICells();
    int32 CountPendingMiniBosses() const;
    void HandleMiniBossPhase(int32 Bracket);
    void HandleBossPhase();
    void TriggerExtraWaves();
    void FinishScenario();
};