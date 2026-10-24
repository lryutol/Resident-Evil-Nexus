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
    // --- DATOS DEL ÍTEM ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
    class UTexture2D* ItemIcon;

    // --- DATOS DEL ARMA (Solo si es tipo Weapon) ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Settings")
    TSubclassOf<class ANexusWeapon> WeaponClass;

    // --- DATOS DE LA CUADRÍCULA (RE4) ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
    FIntPoint GridSize; // Ej: X=2, Y=1 para un arma horizontal
};