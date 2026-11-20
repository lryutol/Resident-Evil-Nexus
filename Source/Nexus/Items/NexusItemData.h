#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NexusItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon UMETA(DisplayName = "Arma"),
    Consumable UMETA(DisplayName = "Consumible"),
    KeyItem UMETA(DisplayName = "Objeto Clave")
};

UCLASS(BlueprintType)
class NEXUS_API UNexusItemData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    class UTexture2D* ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Settings")
    TSubclassOf<class ANexusWeapon> WeaponClass;

    // Slot de la barra inferior: 1 = pistola, 2 = rifle/escopeta
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hotbar")
    int32 HotbarSlot = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
    FIntPoint GridSize;
};