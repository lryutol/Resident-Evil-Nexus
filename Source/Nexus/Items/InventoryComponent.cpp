#include "InventoryComponent.h"
#include "NexusItemData.h"
#include "NexusWeapon.h"
#include "../Characters/NexusCharacter.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentItemIndex = 0;
    EquippedWeapon = nullptr;
}

void UInventoryComponent::AddItem(UNexusItemData* NewItem)
{
    if (NewItem && Items.Num() < 10)
    {
        Items.Add(NewItem);
        UE_LOG(LogTemp, Warning, TEXT("NEXUS: Ítem '%s' añadido al inventario."), *NewItem->ItemName.ToString());

        // Si es el primer ítem, lo equipamos automáticamente
        if (Items.Num() == 1)
        {
            UpdateEquippedWeapon();
        }
    }
}

UNexusItemData* UInventoryComponent::GetCurrentItemData() const
{
    if (Items.IsValidIndex(CurrentItemIndex))
    {
        return Items[CurrentItemIndex];
    }
    return nullptr;
}

void UInventoryComponent::CycleItem(int32 Direction)
{
    if (Items.Num() <= 1) return;

    CurrentItemIndex = (CurrentItemIndex + Direction + Items.Num()) % Items.Num();
    UpdateEquippedWeapon();

    UE_LOG(LogTemp, Warning, TEXT("NEXUS: Cambiando a ítem índice %d"), CurrentItemIndex);
}

void UInventoryComponent::UpdateEquippedWeapon()
{
    ANexusCharacter* Char = Cast<ANexusCharacter>(GetOwner());
    if (!Char) return;

    // 1. Destruimos el arma física anterior si existía
    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
        EquippedWeapon = nullptr;
    }

    // 2. Obtenemos los datos del nuevo ítem
    UNexusItemData* CurrentItem = GetCurrentItemData();
    if (CurrentItem && CurrentItem->ItemType == EItemType::Weapon && CurrentItem->WeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Char;

        // 3. Spawneamos el arma basándonos en la clase que dice el Data Asset
        EquippedWeapon = GetWorld()->SpawnActor<ANexusWeapon>(
            CurrentItem->WeaponClass,
            Char->GetActorLocation(),
            Char->GetActorRotation(),
            SpawnParams
        );

        // 4. La anclamos a la mano
        if (EquippedWeapon)
        {
            EquippedWeapon->AttachToCharacter(Char);
        }
    }
}