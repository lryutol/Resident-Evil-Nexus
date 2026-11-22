#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputCoreTypes.h"
#include "../Patterns/NexusObserver.h"
#include "NexusSubsystem.generated.h"

class UNexusSettingsSave;

UENUM(BlueprintType)
enum class ENexusDifficulty : uint8
{
    Easy   UMETA(DisplayName = "Facil"),
    Medium UMETA(DisplayName = "Medio"),
    Hard   UMETA(DisplayName = "Dificil")
};

UENUM(BlueprintType)
enum class ENexusScenario : uint8
{
    RuinedCity       UMETA(DisplayName = "Ciudad en ruinas"),
    AbandonedFactory UMETA(DisplayName = "Fabrica abandonada"),
    Underground      UMETA(DisplayName = "Subterraneo"),
    Laboratories     UMETA(DisplayName = "Laboratorios exterior")
};

// Multiplicadores que cada dificultad aplica al juego
USTRUCT(BlueprintType)
struct FNexusDifficultyParams
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) float EnemyHealthMultiplier = 1.0f;
    UPROPERTY(BlueprintReadOnly) float EnemyDamageMultiplier = 1.0f;
    UPROPERTY(BlueprintReadOnly) float SpawnRateMultiplier = 1.0f;
    UPROPERTY(BlueprintReadOnly) float PickupMultiplier = 1.0f;
};

// FACHADA (Facade): punto unico de acceso a estado, flujo, dificultad y ajustes
UCLASS(BlueprintType)                                        // <<< CAMBIO 1: BlueprintType añadido
class NEXUS_API UNexusSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // <<< CAMBIO 2: ahora es UFUNCTION con WorldContext para que salga el nodo en Blueprint
    UFUNCTION(BlueprintPure, Category = "Nexus|Facade",
        meta = (WorldContext = "WorldContextObject"))
    static UNexusSubsystem* Get(const UObject* WorldContextObject);

    // ---------- Estado del juego (State + Observer) ----------
    void SetGameState(ENexusGameState NewState);
    ENexusGameState GetGameState() const { return CurrentGameState; }

    void AttachObserver(INexusObserver* Observer);
    void DetachObserver(INexusObserver* Observer);

    // El Objeto Llave llamara a esta funcion
    UFUNCTION(BlueprintCallable, Category = "Nexus|Facade")
    void TriggerBossEvent();

    // ---------- Flujo de partida ----------
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void PrepareCampaign();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void PrepareSingleScenario(ENexusScenario Scenario);
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void ConfirmDifficultyAndStart(ENexusDifficulty NewDifficulty);
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void OnScenarioCompleted();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void RestartCurrentLevel();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void ReturnToMainMenu();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void QuitToDesktop();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Flow") void ShowGameOver();

    // ---------- Dificultad ----------
    UFUNCTION(BlueprintPure, Category = "Nexus|Difficulty") ENexusDifficulty GetDifficulty() const { return Difficulty; }
    UFUNCTION(BlueprintPure, Category = "Nexus|Difficulty") FNexusDifficultyParams GetDifficultyParams() const;

    // ---------- Ajustes ----------
    UFUNCTION(BlueprintPure, Category = "Nexus|Settings") UNexusSettingsSave* GetSettings() const { return Settings; }
    UFUNCTION(BlueprintCallable, Category = "Nexus|Settings") void SaveSettings();
    UFUNCTION(BlueprintCallable, Category = "Nexus|Audio") void ApplyAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Nexus|Screen")
    void ApplyScreenSettings(int32 ResolutionIndex, bool bFullscreen, bool bVSync, int32 QualityLevel);
    UFUNCTION(BlueprintPure, Category = "Nexus|Screen") int32 GetCurrentResolutionIndex() const;
    UFUNCTION(BlueprintPure, Category = "Nexus|Screen") bool IsFullscreen() const;
    UFUNCTION(BlueprintPure, Category = "Nexus|Screen") bool IsVSyncOn() const;
    UFUNCTION(BlueprintPure, Category = "Nexus|Screen") int32 GetQualityLevel() const;

    // ---------- Teclas ----------
    UFUNCTION(BlueprintCallable, Category = "Nexus|Controls") void RebindAction(FName ActionName, FKey NewKey);
    UFUNCTION(BlueprintCallable, Category = "Nexus|Controls") void RebindAxis(FName AxisName, float Scale, FKey NewKey);
    UFUNCTION(BlueprintPure, Category = "Nexus|Controls") FKey GetActionKey(FName ActionName) const;
    UFUNCTION(BlueprintPure, Category = "Nexus|Controls") FKey GetAxisKey(FName AxisName, float Scale) const;
    UFUNCTION(BlueprintCallable, Category = "Nexus|Controls") void ResetKeyBindings();

private:
    void LoadSettings();
    void OpenScenario(int32 Index);
    void OpenMapByName(FName MapName);
    void StoreBinding(FName Name, bool bIsAxis, float Scale, const FKey& Key);
    void NotifyStateChanged();

    ENexusGameState CurrentGameState = ENexusGameState::Exploring;
    TArray<INexusObserver*> Observers;

    UPROPERTY() UNexusSettingsSave* Settings = nullptr;

    ENexusDifficulty Difficulty = ENexusDifficulty::Medium;
    bool bPendingCampaign = true;
    bool bCampaignMode = false;
    bool bGameOverShown = false;
    int32 PendingScenarioIndex = 0;
    int32 CurrentScenarioIndex = 0;
};