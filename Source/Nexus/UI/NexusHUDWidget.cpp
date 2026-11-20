#include "NexusHUDWidget.h"
#include "../Characters/NexusCharacter.h"
#include "../Items/NexusWeapon.h"
#include "../Items/InventoryComponent.h"
#include "../Core/NexusSubsystem.h"
#include "../Core/NexusSettingsSave.h"
#include "../Core/NexusGameDirector.h"
#include "Kismet/GameplayStatics.h"

void UNexusHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->AttachObserver(this);

        // Escala del HUD elegida en el menu de configuracion
        if (Fachada->GetSettings())
        {
            const float HudScale = Fachada->GetSettings()->HudScale;
            SetRenderScale(FVector2D(HudScale, HudScale));
        }
    }

    GameDirector = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ANexusGameDirector::StaticClass()));
    if (GameDirector)
    {
        GameDirector->AttachUI(this);
        SyncFromDirector();
    }

    PlayerChar = Cast<ANexusCharacter>(GetOwningPlayerPawn());
    if (PlayerChar)
    {
        PlayerChar->AttachObserver(this);
        BP_OnHealthUpdated(PlayerChar->GetCurrentHealth() / PlayerChar->GetMaxHealth());
    }
}

void UNexusHUDWidget::NativeDestruct()
{
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this)) Fachada->DetachObserver(this);
    if (GameDirector) GameDirector->DetachUI(this);
    if (PlayerChar) PlayerChar->DetachObserver(this);
    if (CurrentWeapon) CurrentWeapon->DetachObserver(this);

    Super::NativeDestruct();
}

// Envia los valores actuales del Director al HUD (para el estado inicial)
void UNexusHUDWidget::SyncFromDirector()
{
    if (!GameDirector) return;

    BP_OnKillCountUpdated(GameDirector->GetKillCount(), GameDirector->GetKillGoal());
    BP_OnScoreUpdated(GameDirector->GetScore(), GameDirector->GetComboCount());
    BP_OnCellsUpdated(GameDirector->GetCellsRemaining(), GameDirector->GetCellsTotal());
}

void UNexusHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Director: si aun no existia al crear el HUD, lo buscamos
    if (!GameDirector)
    {
        GameDirector = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ANexusGameDirector::StaticClass()));
        if (GameDirector)
        {
            GameDirector->AttachUI(this);
            SyncFromDirector();
        }
    }

    if (GameDirector)
    {
        BP_OnTimerUpdated(GameDirector->GetTimeRemaining(), GameDirector->GetComboRatio(), GameDirector->GetComboMultiplier());
    }

    // Jugador: si aun no existia, lo buscamos
    if (!PlayerChar)
    {
        PlayerChar = Cast<ANexusCharacter>(GetOwningPlayerPawn());
        if (PlayerChar)
        {
            PlayerChar->AttachObserver(this);
            BP_OnHealthUpdated(PlayerChar->GetCurrentHealth() / PlayerChar->GetMaxHealth());
        }
    }
    if (!PlayerChar) return;

    UInventoryComponent* Inv = PlayerChar->GetInventory();
    if (!Inv) return;

    // Observer: si el arma equipada cambio, nos suscribimos a la nueva
    ANexusWeapon* Equipped = Inv->GetEquippedWeapon();
    if (Equipped != CurrentWeapon)
    {
        if (CurrentWeapon) CurrentWeapon->DetachObserver(this);
        CurrentWeapon = Equipped;

        if (CurrentWeapon)
        {
            CurrentWeapon->AttachObserver(this);
            BP_OnAmmoUpdated(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetReserveAmmo());
        }
    }

    // Barra de slots (OJO: "Slot" ya existe en UUserWidget, por eso se llama SelectedSlot)
    const int32 SelectedSlot = Inv->GetEquippedSlot();
    const int32 Herbs = Inv->GetHerbCount();
    const int32 Grenades = Inv->GetGrenadeCount();
    if (SelectedSlot != LastSlot || Herbs != LastHerbs || Grenades != LastGrenades)
    {
        LastSlot = SelectedSlot;
        LastHerbs = Herbs;
        LastGrenades = Grenades;
        BP_OnHotbarChanged(SelectedSlot, Herbs, Grenades);
    }
}

void UNexusHUDWidget::OnGameStateChanged(ENexusGameState NewState) { BP_OnGameStateChanged(NewState); }
void UNexusHUDWidget::OnHealthChanged(float CurrentHealth, float MaxHealth) { BP_OnHealthUpdated((MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.f); }
void UNexusHUDWidget::OnAmmoChanged(int32 CurrentAmmo, int32 ReserveAmmo) { BP_OnAmmoUpdated(CurrentAmmo, ReserveAmmo); }
void UNexusHUDWidget::OnEnemyCountChanged(int32 CurrentKills, int32 MaxKills) { BP_OnKillCountUpdated(CurrentKills, MaxKills); }
void UNexusHUDWidget::OnScoreChanged(int32 CurrentScore, int32 ComboCount) { BP_OnScoreUpdated(CurrentScore, ComboCount); }
void UNexusHUDWidget::OnTimeBonus(float Seconds) { BP_OnTimeBonus(Seconds); }
void UNexusHUDWidget::OnCellsChanged(int32 CellsRemaining, int32 CellsTotal) { BP_OnCellsUpdated(CellsRemaining, CellsTotal); }