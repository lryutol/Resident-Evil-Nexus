#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UNexusItemData;
class ANexusWeapon;
class ANexusCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEXUS_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(UNexusItemData* NewItem);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UNexusItemData* GetCurrentItemData() const;

    void CycleItem(int32 Direction);

    // Getter para que el personaje pueda acceder al arma física
    ANexusWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<UNexusItemData*> Items;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 CurrentItemIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    ANexusWeapon* EquippedWeapon;

    void UpdateEquippedWeapon();
};