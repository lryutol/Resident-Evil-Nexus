#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NexusPlayerController.generated.h"

class UUserWidget;
class UNexusInventoryWidget;
class UNexusHUDWidget;

UCLASS()
class NEXUS_API ANexusPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ANexusPlayerController();
    void ToggleCrosshair(bool bShow);
    void ToggleInventory();

    UFUNCTION(BlueprintCallable, Category = "Pause") void TogglePauseMenu();
    UFUNCTION(BlueprintCallable, Category = "Pause") void ResumeGame();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override; // <-- Añadido para verificar Game Over

    UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UUserWidget> CrosshairWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UNexusInventoryWidget> InventoryWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UNexusHUDWidget> HUDWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UUserWidget> PauseWidgetClass;

private:
    UPROPERTY() UUserWidget* CrosshairWidget;
    UPROPERTY() UNexusInventoryWidget* InventoryWidget;
    UPROPERTY() UNexusHUDWidget* HUDWidget;
    UPROPERTY() UUserWidget* PauseWidget;

    bool bIsInventoryOpen;
    bool bIsPauseOpen;

    bool bMusicTriggered; // Para evitar que la música suene múltiples veces
};