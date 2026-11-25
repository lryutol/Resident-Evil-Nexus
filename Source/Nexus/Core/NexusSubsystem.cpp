#include "NexusSubsystem.h"
#include "NexusSettingsSave.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

namespace
{
    const int32 ScenarioCount = 4;
    const float LoadingPollInterval = 0.03f;

    const FIntPoint Resolutions[] = {
        FIntPoint(1280, 720), FIntPoint(1600, 900), FIntPoint(1920, 1080), FIntPoint(2560, 1440)
    };

    FString SettingsSlot() { return TEXT("NexusSettings"); }
    FName MainMenuMap() { return FName(TEXT("/Game/Mapas/MainMenu")); }
    const TCHAR* LoadingWidgetPath() { return TEXT("/Game/UI/Menu/WBP_LoadingScreen.WBP_LoadingScreen_C"); }

    FName GetScenarioMap(int32 Index)
    {
        switch (Index)
        {
        case 0:  return FName(TEXT("/Game/Mapas/Nexus_Level"));
        case 1:  return FName(TEXT("/Game/AbandonedPowerPlant/Maps/PowerPlant"));
        case 2:  return FName(TEXT("/Game/UndergroundTunnel/Maps/Demonstration"));
        default: return FName(TEXT("/Game/ModularSciFiStation/Level/SciFiStationExampleMap"));
        }
    }

    void ApplyActionKey(FName Action, const FKey& Key)
    {
        UInputSettings* S = GetMutableDefault<UInputSettings>();
        TArray<FInputActionKeyMapping> Old;
        S->GetActionMappingByName(Action, Old);
        for (const FInputActionKeyMapping& M : Old)
        {
            if (!M.Key.IsGamepadKey()) { S->RemoveActionMapping(M, false); }
        }
        S->AddActionMapping(FInputActionKeyMapping(Action, Key), true);
    }

    void ApplyAxisKey(FName Axis, float Scale, const FKey& Key)
    {
        UInputSettings* S = GetMutableDefault<UInputSettings>();
        TArray<FInputAxisKeyMapping> Old;
        S->GetAxisMappingByName(Axis, Old);
        for (const FInputAxisKeyMapping& M : Old)
        {
            if (!M.Key.IsGamepadKey() && FMath::IsNearlyEqual(M.Scale, Scale)) { S->RemoveAxisMapping(M, false); }
        }
        S->AddAxisMapping(FInputAxisKeyMapping(Axis, Key, Scale), true);
    }
}

void UNexusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentGameState = ENexusGameState::Exploring;
    LoadSettings();

    if (GEngine && GEngine->GetGameUserSettings())
    {
        UGameUserSettings* GS = GEngine->GetGameUserSettings();
        if (GS->GetOverallScalabilityLevel() < 2)
        {
            GS->SetOverallScalabilityLevel(3);
            GS->ApplySettings(false);
        }
    }
}

UNexusSubsystem* UNexusSubsystem::Get(const UObject* WorldContextObject)
{
    if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        return GameInstance->GetSubsystem<UNexusSubsystem>();
    }
    return nullptr;
}

void UNexusSubsystem::SetGameState(ENexusGameState NewState)
{
    if (CurrentGameState == NewState) return;
    CurrentGameState = NewState;
    NotifyStateChanged();
}

void UNexusSubsystem::AttachObserver(INexusObserver* Observer)
{
    if (Observer && !Observers.Contains(Observer)) Observers.Add(Observer);
}

void UNexusSubsystem::DetachObserver(INexusObserver* Observer)
{
    if (Observer) Observers.Remove(Observer);
}

void UNexusSubsystem::NotifyStateChanged()
{
    TArray<INexusObserver*> Copy = Observers;
    for (INexusObserver* Obs : Copy)
    {
        if (Obs) Obs->OnGameStateChanged(CurrentGameState);
    }
}

void UNexusSubsystem::TriggerBossEvent()
{
    SetGameState(ENexusGameState::BossFight);
    UE_LOG(LogTemp, Warning, TEXT("FACHADA: Evento Llave activado! El Spawner sabe que hacer."));
}

void UNexusSubsystem::PrepareCampaign()
{
    bPendingCampaign = true;
}

void UNexusSubsystem::PrepareSingleScenario(ENexusScenario Scenario)
{
    bPendingCampaign = false;
    PendingScenarioIndex = static_cast<int32>(Scenario);
}

void UNexusSubsystem::ConfirmDifficultyAndStart(ENexusDifficulty NewDifficulty)
{
    Difficulty = NewDifficulty;
    bCampaignMode = bPendingCampaign;
    OpenScenario(bCampaignMode ? 0 : PendingScenarioIndex);
}

void UNexusSubsystem::OnScenarioCompleted()
{
    if (bCampaignMode && CurrentScenarioIndex + 1 < ScenarioCount)
    {
        OpenScenario(CurrentScenarioIndex + 1);
    }
    else
    {
        ReturnToMainMenu();
    }
}

void UNexusSubsystem::RestartCurrentLevel()
{
    if (UWorld* World = GetWorld())
    {
        const FString Path = UWorld::RemovePIEPrefix(World->GetOutermost()->GetName());
        OpenMapByName(FName(*Path), CurrentScenarioIndex);
    }
}

void UNexusSubsystem::ReturnToMainMenu()
{
    bCampaignMode = false;
    OpenMapByName(MainMenuMap(), -1);
}

void UNexusSubsystem::QuitToDesktop()
{
    UGameInstance* GI = GetGameInstance();
    APlayerController* PC = GI ? GI->GetFirstLocalPlayerController() : nullptr;
    UKismetSystemLibrary::QuitGame(GI, PC, EQuitPreference::Quit, false);
}

void UNexusSubsystem::ShowGameOver()
{
    UGameInstance* GI = GetGameInstance();
    APlayerController* PC = GI ? GI->GetFirstLocalPlayerController() : nullptr;
    if (bGameOverShown || !PC) return;

    UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/Menu/WBP_GameOver.WBP_GameOver_C"));
    if (!WidgetClass) return;

    UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
    if (!Widget) return;

    bGameOverShown = true;
    Widget->AddToViewport(100);

    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(Widget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(Mode);
    PC->bShowMouseCursor = true;

    UGameplayStatics::SetGamePaused(GI, true);
}

void UNexusSubsystem::OpenScenario(int32 Index)
{
    CurrentScenarioIndex = FMath::Clamp(Index, 0, ScenarioCount - 1);
    OpenMapByName(GetScenarioMap(CurrentScenarioIndex), CurrentScenarioIndex);
}

void UNexusSubsystem::OpenMapByName(FName MapName, int32 ScenarioIdxForLoading)
{
    if (bIsLoadingMap || MapName.IsNone()) return;

    bGameOverShown = false;
    UGameInstance* GI = GetGameInstance();
    UGameplayStatics::SetGamePaused(GI, false);

    ShowLoadingAndTravel(MapName, ScenarioIdxForLoading);
}

void UNexusSubsystem::ShowLoadingAndTravel(FName MapName, int32 ScenarioIdxForLoading)
{
    bIsLoadingMap = true;
    bPackageReadyToOpen = false;
    PendingMapName = MapName;
    TargetScenarioIndex = ScenarioIdxForLoading;
    LoadingProgress = 0.05f;

    UGameInstance* GI = GetGameInstance();
    APlayerController* PC = GI ? GI->GetFirstLocalPlayerController() : nullptr;
    UClass* LoadingClass = LoadClass<UUserWidget>(nullptr, LoadingWidgetPath());

    if (PC && LoadingClass)
    {
        if (!ActiveLoadingWidget)
        {
            ActiveLoadingWidget = CreateWidget<UUserWidget>(PC, LoadingClass);
        }
        if (ActiveLoadingWidget)
        {
            if (!ActiveLoadingWidget->IsInViewport())
            {
                ActiveLoadingWidget->AddToViewport(999);
            }
            ActiveLoadingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            PC->bShowMouseCursor = false;
            PC->SetInputMode(FInputModeGameOnly());
        }
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            LoadingTickTimerHandle,
            this,
            &UNexusSubsystem::PollAsyncLoadProgress,
            LoadingPollInterval,
            true);
    }

    FLoadPackageAsyncDelegate LoadedDelegate;
    LoadedDelegate.BindUObject(this, &UNexusSubsystem::OnMapPackageLoaded);
    LoadPackageAsync(MapName.ToString(), LoadedDelegate, 0, PKG_None, INDEX_NONE);
}

void UNexusSubsystem::PollAsyncLoadProgress()
{
    if (!bIsLoadingMap || PendingMapName.IsNone())
    {
        StopLoadingTimer();
        return;
    }

    if (bPackageReadyToOpen)
    {
        LoadingProgress = 1.0f;
        StopLoadingTimer();
        ExecuteOpenLevelNow(PendingMapName);
        return;
    }

    const float AsyncPct = GetAsyncLoadPercentage(PendingMapName);
    if (AsyncPct >= 0.0f)
    {
        LoadingProgress = FMath::Clamp(AsyncPct / 100.0f, 0.05f, 0.99f);
    }
    else
    {
        LoadingProgress = FMath::Clamp(LoadingProgress + 0.05f, 0.05f, 0.95f);
    }
}

void UNexusSubsystem::OnMapPackageLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
{
    if (!bIsLoadingMap || PendingMapName.IsNone()) return;

    bPackageReadyToOpen = true;
    LoadingProgress = 1.0f;
    StopLoadingTimer();

    const FName MapToOpen = PendingMapName;
    ExecuteOpenLevelNow(MapToOpen);
}

void UNexusSubsystem::ExecuteOpenLevelNow(FName MapToOpen)
{
    if (!bIsLoadingMap || MapToOpen.IsNone()) return;

    bIsLoadingMap = false;
    bPackageReadyToOpen = false;
    PendingMapName = NAME_None;

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UGameplayStatics::OpenLevel(GI, MapToOpen);
    }
}

void UNexusSubsystem::HideLoadingScreen()
{
    StopLoadingTimer();
    bIsLoadingMap = false;
    bPackageReadyToOpen = false;
    LoadingProgress = 0.0f;

    if (ActiveLoadingWidget)
    {
        ActiveLoadingWidget->RemoveFromParent();
        ActiveLoadingWidget = nullptr;
    }
}

void UNexusSubsystem::StopLoadingTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(LoadingTickTimerHandle);
    }
}

FNexusDifficultyParams UNexusSubsystem::GetDifficultyParams() const
{
    FNexusDifficultyParams P;
    switch (Difficulty)
    {
    case ENexusDifficulty::Easy:
        P.EnemyHealthMultiplier = 0.7f; P.EnemyDamageMultiplier = 0.7f;
        P.SpawnRateMultiplier = 0.75f;  P.PickupMultiplier = 1.5f;
        break;
    case ENexusDifficulty::Hard:
        P.EnemyHealthMultiplier = 1.5f; P.EnemyDamageMultiplier = 1.5f;
        P.SpawnRateMultiplier = 1.4f;   P.PickupMultiplier = 0.6f;
        break;
    default:
        break;
    }
    return P;
}

void UNexusSubsystem::LoadSettings()
{
    Settings = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(SettingsSlot(), 0))
    {
        Settings = Cast<UNexusSettingsSave>(UGameplayStatics::LoadGameFromSlot(SettingsSlot(), 0));
    }
    if (!Settings)
    {
        Settings = Cast<UNexusSettingsSave>(UGameplayStatics::CreateSaveGameObject(UNexusSettingsSave::StaticClass()));
    }
    if (!Settings) return;

    for (const FNexusKeyBinding& B : Settings->KeyBindings)
    {
        if (B.bIsAxis) { ApplyAxisKey(B.Name, B.Scale, B.Key); }
        else { ApplyActionKey(B.Name, B.Key); }
    }
}

void UNexusSubsystem::SaveSettings()
{
    if (Settings) { UGameplayStatics::SaveGameToSlot(Settings, SettingsSlot(), 0); }
}

void UNexusSubsystem::ApplyAudioSettings()
{
    UWorld* World = GetWorld();
    if (!World || !Settings) return;

    USoundMix* Mix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/SM_Master.SM_Master"));
    USoundClass* Master = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SC_Master.SC_Master"));
    USoundClass* Music = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SC_Music.SC_Music"));
    USoundClass* Sfx = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SC_SFX.SC_SFX"));
    if (!Mix) return;

    UGameplayStatics::PushSoundMixModifier(World, Mix);
    if (Master) UGameplayStatics::SetSoundMixClassOverride(World, Mix, Master, Settings->MasterVolume, 1.0f, 0.1f, true);
    if (Music)  UGameplayStatics::SetSoundMixClassOverride(World, Mix, Music, Settings->MusicVolume, 1.0f, 0.1f, true);
    if (Sfx)    UGameplayStatics::SetSoundMixClassOverride(World, Mix, Sfx, Settings->SfxVolume, 1.0f, 0.1f, true);
}

void UNexusSubsystem::ApplyScreenSettings(int32 ResolutionIndex, bool bFullscreen, bool bVSync, int32 QualityLevel)
{
    if (!GEngine) return;
    UGameUserSettings* GS = GEngine->GetGameUserSettings();
    if (!GS) return;

    GS->SetScreenResolution(Resolutions[FMath::Clamp(ResolutionIndex, 0, 3)]);
    GS->SetFullscreenMode(bFullscreen ? EWindowMode::WindowedFullscreen : EWindowMode::Windowed);
    GS->SetVSyncEnabled(bVSync);
    GS->SetOverallScalabilityLevel(FMath::Clamp(QualityLevel, 0, 3));
    GS->ApplySettings(false);
    GS->SaveSettings();
}

int32 UNexusSubsystem::GetCurrentResolutionIndex() const
{
    if (GEngine && GEngine->GetGameUserSettings())
    {
        const FIntPoint Cur = GEngine->GetGameUserSettings()->GetScreenResolution();
        for (int32 i = 0; i < 4; ++i)
        {
            if (Resolutions[i] == Cur) return i;
        }
    }
    return 2;
}

bool UNexusSubsystem::IsFullscreen() const
{
    return GEngine && GEngine->GetGameUserSettings()
        && GEngine->GetGameUserSettings()->GetFullscreenMode() != EWindowMode::Windowed;
}

bool UNexusSubsystem::IsVSyncOn() const
{
    return GEngine && GEngine->GetGameUserSettings() && GEngine->GetGameUserSettings()->IsVSyncEnabled();
}

int32 UNexusSubsystem::GetQualityLevel() const
{
    if (GEngine && GEngine->GetGameUserSettings())
    {
        return FMath::Clamp(GEngine->GetGameUserSettings()->GetOverallScalabilityLevel(), 0, 3);
    }
    return 2;
}

void UNexusSubsystem::StoreBinding(FName Name, bool bIsAxis, float Scale, const FKey& Key)
{
    if (!Settings) return;
    for (FNexusKeyBinding& B : Settings->KeyBindings)
    {
        if (B.Name == Name && B.bIsAxis == bIsAxis && FMath::IsNearlyEqual(B.Scale, Scale))
        {
            B.Key = Key;
            return;
        }
    }
    FNexusKeyBinding NewBinding;
    NewBinding.Name = Name;
    NewBinding.bIsAxis = bIsAxis;
    NewBinding.Scale = Scale;
    NewBinding.Key = Key;
    Settings->KeyBindings.Add(NewBinding);
}

void UNexusSubsystem::RebindAction(FName ActionName, FKey NewKey)
{
    ApplyActionKey(ActionName, NewKey);
    StoreBinding(ActionName, false, 1.0f, NewKey);
    SaveSettings();
}

void UNexusSubsystem::RebindAxis(FName AxisName, float Scale, FKey NewKey)
{
    ApplyAxisKey(AxisName, Scale, NewKey);
    StoreBinding(AxisName, true, Scale, NewKey);
    SaveSettings();
}

FKey UNexusSubsystem::GetActionKey(FName ActionName) const
{
    TArray<FInputActionKeyMapping> M;
    GetDefault<UInputSettings>()->GetActionMappingByName(ActionName, M);
    for (const FInputActionKeyMapping& X : M)
    {
        if (!X.Key.IsGamepadKey()) return X.Key;
    }
    return EKeys::Invalid;
}

FKey UNexusSubsystem::GetAxisKey(FName AxisName, float Scale) const
{
    TArray<FInputAxisKeyMapping> M;
    GetDefault<UInputSettings>()->GetAxisMappingByName(AxisName, M);
    for (const FInputAxisKeyMapping& X : M)
    {
        if (!X.Key.IsGamepadKey() && FMath::IsNearlyEqual(X.Scale, Scale)) return X.Key;
    }
    return EKeys::Invalid;
}

void UNexusSubsystem::ResetKeyBindings()
{
    UInputSettings* S = GetMutableDefault<UInputSettings>();
    S->ReloadConfig();
    S->ForceRebuildKeymaps();
    if (Settings) { Settings->KeyBindings.Empty(); }
    SaveSettings();
}