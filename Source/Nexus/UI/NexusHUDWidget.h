#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Patterns/NexusObserver.h"
#include "NexusHUDWidget.generated.h"

UCLASS()
class NEXUS_API UNexusHUDWidget : public UUserWidget, public INexusObserver
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // --- INTERFAZ OBSERVER ---
    virtual void OnGameStateChanged(ENexusGameState NewState) override;
    virtual void OnHealthChanged(float CurrentHealth, float MaxHealth) override;
    virtual void OnAmmoChanged(int32 CurrentAmmo, int32 ReserveAmmo) override;
    virtual void OnEnemyCountChanged(int32 CurrentKills, int32 MaxKills) override;
    virtual void OnScoreChanged(int32 CurrentScore, int32 ComboCount) override;
    virtual void OnTimeBonus(float Seconds) override;
    virtual void OnCellsChanged(int32 CellsRemaining, int32 CellsTotal) override;

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnGameStateChanged(ENexusGameState NewState);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnHealthUpdated(float HealthPercent);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnAmmoUpdated(int32 Ammo, int32 ReserveAmmo);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnKillCountUpdated(int32 CurrentKills, int32 MaxKills);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnScoreUpdated(int32 CurrentScore, int32 ComboCount);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnTimeBonus(float Seconds);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnCellsUpdated(int32 CellsRemaining, int32 CellsTotal);

    // Se dispara cada frame: tiempo restante, barra de racha y multiplicador
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnTimerUpdated(float TimeRemaining, float ComboRatio, float Multiplier);

    // Barra de slots: slot equipado (0 = ninguno) y cantidades
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD") void BP_OnHotbarChanged(int32 SelectedSlot, int32 HerbCount, int32 GrenadeCount);

private:
    void SyncFromDirector();

    UPROPERTY()
    class ANexusCharacter* PlayerChar = nullptr;

    UPROPERTY()
    class ANexusWeapon* CurrentWeapon = nullptr;

    UPROPERTY()
    class ANexusGameDirector* GameDirector = nullptr;

    int32 LastSlot = -1;
    int32 LastHerbs = -1;
    int32 LastGrenades = -1;
};