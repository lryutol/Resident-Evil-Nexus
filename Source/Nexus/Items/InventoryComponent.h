#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NexusWeapon.h"
#include "InventoryComponent.generated.h"

class UNexusItemData;
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

    // Slots de la barra: 1 = pistola, 2 = rifle/escopeta (pulsar otra vez alterna)
    void SelectSlot(int32 Slot);

    // Pickups
    bool AddAmmo(ENexusAmmoType Type, int32 Magazines);
    bool AddHerb(int32 Amount);
    bool AddGrenade(int32 Amount);

    // Usa una hierba: cura al jugador. False si no hay o ya tiene la vida llena
    bool UseHerb();

    ANexusWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
    int32 GetEquippedSlot() const;
    int32 GetHerbCount() const { return HerbCount; }
    int32 GetGrenadeCount() const { return GrenadeCount; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<UNexusItemData*> Items;

    // Un arma fisica por cada item (nullptr si el item no es arma)
    UPROPERTY()
    TArray<ANexusWeapon*> SpawnedWeapons;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 CurrentItemIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    ANexusWeapon* EquippedWeapon;

    int32 HerbCount;
    int32 GrenadeCount;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float HerbHealAmount;

    void EquipIndex(int32 Index);
};