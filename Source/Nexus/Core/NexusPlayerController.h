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

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CrosshairWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UNexusInventoryWidget> InventoryWidgetClass;

private:
    UPROPERTY()
    UUserWidget* CrosshairWidget;

    UPROPERTY()
    UNexusInventoryWidget* InventoryWidget;

    bool bIsInventoryOpen;
};