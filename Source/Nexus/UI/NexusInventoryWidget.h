#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NexusInventoryWidget.generated.h"

class UInventoryComponent;

UCLASS()
class NEXUS_API UNexusInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

    void SetupInventory(UInventoryComponent* InInventory);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;
};