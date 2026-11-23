#include "InventoryComponent.h"
#include "NexusItemData.h"
#include "../Characters/NexusCharacter.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentItemIndex = 0;
    EquippedWeapon = nullptr;
    HerbCount = 0;
    GrenadeCount = 0;
    HerbHealAmount = 40.f;
}

void UInventoryComponent::AddItem(UNexusItemData* NewItem)
{
    if (!NewItem || Items.Num() >= 10) return;

    ANexusCharacter* Char = Cast<ANexusCharacter>(GetOwner());
    ANexusWeapon* Spawned = nullptr;

    // Cada arma se crea UNA sola vez (asi la municion no se reinicia al cambiar)
    if (Char && NewItem->ItemType == EItemType::Weapon && NewItem->WeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Char;

        Spawned = GetWorld()->SpawnActor<ANexusWeapon>(
            NewItem->WeaponClass,
            Char->GetActorLocation(),
            Char->GetActorRotation(),
            SpawnParams
        );

        if (Spawned)
        {
            Spawned->AttachToCharacter(Char);
            Spawned->SetEquipped(false);
        }
    }

    if (NewItem->ItemType == EItemType::Weapon && !Spawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("INVENTARIO: '%s' es un arma pero no se pudo crear. Revisa 'Weapon Class' en su Data Asset."), *NewItem->ItemName.ToString());
    }

    Items.Add(NewItem);
    SpawnedWeapons.Add(Spawned);
    UE_LOG(LogTemp, Warning, TEXT("INVENTARIO: '%s' anadido (slot de barra %d)."), *NewItem->ItemName.ToString(), NewItem->HotbarSlot);

    if (Items.Num() == 1)
    {
        EquipIndex(0);
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

void UInventoryComponent::EquipIndex(int32 Index)
{
    for (ANexusWeapon* W : SpawnedWeapons)
    {
        if (W) W->SetEquipped(false);
    }

    CurrentItemIndex = Index;
    EquippedWeapon = SpawnedWeapons.IsValidIndex(Index) ? SpawnedWeapons[Index] : nullptr;

    if (EquippedWeapon)
    {
        EquippedWeapon->SetEquipped(true);
    }

    const FString WeaponName = EquippedWeapon ? EquippedWeapon->GetName() : FString(TEXT("NINGUNA"));
    UE_LOG(LogTemp, Warning, TEXT("INVENTARIO: equipado el indice %d, arma: %s"), Index, *WeaponName);
}

void UInventoryComponent::CycleItem(int32 Direction)
{
    if (Items.Num() <= 1) return;

    EquipIndex((CurrentItemIndex + Direction + Items.Num()) % Items.Num());
}

void UInventoryComponent::SelectSlot(int32 Slot)
{
    int32 First = -1;
    int32 Next = -1;

    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i] && Items[i]->HotbarSlot == Slot)
        {
            if (First < 0) First = i;
            if (i > CurrentItemIndex && Next < 0) Next = i;
        }
    }

    if (First < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("INVENTARIO: ningun item tiene Hotbar Slot = %d."), Slot);
        return;
    }

    const bool bCurrentInSlot = Items.IsValidIndex(CurrentItemIndex)
        && Items[CurrentItemIndex]
        && Items[CurrentItemIndex]->HotbarSlot == Slot;

    if (!bCurrentInSlot)
    {
        EquipIndex(First);
    }
    else
    {
        // Ya estamos en ese slot: alternamos (rifle <-> escopeta)
        EquipIndex(Next >= 0 ? Next : First);
    }
}

int32 UInventoryComponent::GetEquippedSlot() const
{
    if (Items.IsValidIndex(CurrentItemIndex) && Items[CurrentItemIndex])
    {
        return Items[CurrentItemIndex]->HotbarSlot;
    }
    return 0;
}

bool UInventoryComponent::AddAmmo(ENexusAmmoType Type, int32 Magazines)
{
    for (ANexusWeapon* W : SpawnedWeapons)
    {
        if (W && W->GetAmmoType() == Type)
        {
            return W->AddReserveAmmo(Magazines * W->GetMaxAmmo());
        }
    }
    return false;
}

bool UInventoryComponent::AddHerb(int32 Amount)
{
    if (HerbCount >= 9) return false;
    HerbCount = FMath::Min(9, HerbCount + Amount);
    return true;
}

bool UInventoryComponent::AddGrenade(int32 Amount)
{
    if (GrenadeCount >= 5) return false;
    GrenadeCount = FMath::Min(5, GrenadeCount + Amount);
    return true;
}

bool UInventoryComponent::UseHerb()
{
    ANexusCharacter* Char = Cast<ANexusCharacter>(GetOwner());
    if (!Char || HerbCount <= 0) return false;
    if (Char->GetCurrentHealth() >= Char->GetMaxHealth()) return false;

    Char->Heal(HerbHealAmount);
    HerbCount--;
    return true;
}

bool UInventoryComponent::UseGrenade()
{
    if (GrenadeCount > 0)
    {
        GrenadeCount--;
        return true;
    }
    return false;
}