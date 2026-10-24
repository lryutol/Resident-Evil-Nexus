#include "NexusPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../UI/NexusInventoryWidget.h"
#include "../Characters/NexusCharacter.h"
#include "../Items/InventoryComponent.h"

ANexusPlayerController::ANexusPlayerController()
{
    CrosshairWidget = nullptr;
    InventoryWidget = nullptr;
    bIsInventoryOpen = false;
}

void ANexusPlayerController::BeginPlay()
{
    Super::BeginPlay();

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

void ANexusPlayerController::ToggleCrosshair(bool bShow)
{
    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void ANexusPlayerController::ToggleInventory()
{
    // Si el widget no existe, lo creamos
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