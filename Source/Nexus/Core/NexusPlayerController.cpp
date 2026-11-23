#include "NexusPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "../UI/NexusInventoryWidget.h"
#include "../UI/NexusHUDWidget.h"
#include "../Characters/NexusCharacter.h"
#include "../Items/InventoryComponent.h"
#include "NexusSubsystem.h"
#include "NexusSettingsSave.h"
#include "NexusGameMode.h" // Para llamar a la música
#include "Kismet/GameplayStatics.h"

ANexusPlayerController::ANexusPlayerController()
{
    CrosshairWidget = nullptr;
    InventoryWidget = nullptr;
    HUDWidget = nullptr;
    PauseWidget = nullptr;
    bIsInventoryOpen = false;
    bIsPauseOpen = false;
    bMusicTriggered = false;
}

void ANexusPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly GameMode;
    SetInputMode(GameMode);
    bShowMouseCursor = false;

    if (!IsLocalController()) return;

    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UNexusHUDWidget>(this, HUDWidgetClass);
        if (HUDWidget) HUDWidget->AddToViewport(0);
    }

    if (CrosshairWidgetClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport();
            CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void ANexusPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Verificamos si el juego terminó para cambiar la música
    if (!bMusicTriggered)
    {
        if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
        {
            ENexusGameState State = Fachada->GetGameState();
            if (State == ENexusGameState::GameOver || State == ENexusGameState::LevelComplete)
            {
                if (ANexusGameMode* GM = Cast<ANexusGameMode>(UGameplayStatics::GetGameMode(this)))
                {
                    GM->PlayEndGameMusic(State == ENexusGameState::LevelComplete);
                    bMusicTriggered = true;
                }
            }
        }
    }
}

void ANexusPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        // Tecla P (Pause) y Escape (Cancel)
        FInputActionBinding& PauseBinding = InputComponent->BindAction("Pause", IE_Pressed, this, &ANexusPlayerController::TogglePauseMenu);
        PauseBinding.bExecuteWhenPaused = true;

        FInputActionBinding& CancelBinding = InputComponent->BindAction("Cancel", IE_Pressed, this, &ANexusPlayerController::TogglePauseMenu);
        CancelBinding.bExecuteWhenPaused = true;
    }
}

void ANexusPlayerController::ToggleCrosshair(bool bShow)
{
    if (!CrosshairWidget) return;
    bool bAllowed = true;
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        if (Fachada->GetSettings()) bAllowed = Fachada->GetSettings()->bShowCrosshair;
    }
    CrosshairWidget->SetVisibility((bShow && bAllowed) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ANexusPlayerController::ToggleInventory()
{
    if (bIsPauseOpen) return;

    if (!InventoryWidget && InventoryWidgetClass)
    {
        ANexusCharacter* Char = Cast<ANexusCharacter>(GetPawn());
        if (Char)
        {
            UInventoryComponent* InvComp = Char->FindComponentByClass<UInventoryComponent>();
            if (InvComp)
            {
                InventoryWidget = CreateWidget<UNexusInventoryWidget>(this, InventoryWidgetClass);
                if (InventoryWidget)
                {
                    InventoryWidget->SetupInventory(InvComp);
                    InventoryWidget->AddToViewport();
                }
            }
        }
    }

    if (!InventoryWidget) return;
    bIsInventoryOpen = !bIsInventoryOpen;

    if (bIsInventoryOpen)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        SetPause(true);
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
        bShowMouseCursor = true;
    }
    else
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
        SetPause(false);
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = false;
    }
}

void ANexusPlayerController::TogglePauseMenu()
{
    if (bIsInventoryOpen) return;

    if (bIsPauseOpen)
    {
        ResumeGame();
        return;
    }

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        if (Fachada->GetGameState() == ENexusGameState::GameOver) return;
    }

    TSubclassOf<UUserWidget> WidgetToUse = PauseWidgetClass;
    if (!WidgetToUse)
    {
        // Si no asignas nada en el BP, busca esta ruta por defecto
        WidgetToUse = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/Menu/WBP_PauseMenu.WBP_PauseMenu_C"));
    }
    if (!WidgetToUse) return;

    PauseWidget = CreateWidget<UUserWidget>(this, WidgetToUse);
    if (!PauseWidget) return;

    PauseWidget->AddToViewport(50);
    bIsPauseOpen = true;
    SetPause(true);

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(PauseWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
}

void ANexusPlayerController::ResumeGame()
{
    if (!bIsPauseOpen) return;
    bIsPauseOpen = false;

    if (PauseWidget)
    {
        PauseWidget->RemoveFromParent();
        PauseWidget = nullptr;
    }

    SetPause(false);
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
}