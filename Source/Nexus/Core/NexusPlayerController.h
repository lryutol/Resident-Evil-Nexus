#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NexusPlayerController.generated.h"

class UUserWidget;
class UNexusInventoryWidget;

UCLASS()
class NEXUS_API ANexusPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ANexusPlayerController();
    void ToggleCrosshair(bool bShow);
    void ToggleInventory();

protected:
    virtual void BeginPlay() override;

    // <<< AÑADIDO: Variable para asignar el WBP_HUD en el editor >>>
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CrosshairWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNexusInventoryWidget> InventoryWidgetClass;

private:
    // <<< AÑADIDO: Puntero para guardar el HUD creado >>>
    UPROPERTY()
    UUserWidget* HUDWidget;

    UPROPERTY()
    UUserWidget* CrosshairWidget;

    UPROPERTY()
    UNexusInventoryWidget* InventoryWidget;

    bool bIsInventoryOpen;
};