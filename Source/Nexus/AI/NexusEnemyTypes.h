#pragma once

#include "CoreMinimal.h"
#include "NexusEnemyTypes.generated.h"

UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Common      UMETA(DisplayName = "Zombie Comun"),
    Special     UMETA(DisplayName = "Zombie Especial"),
    BossT1104   UMETA(DisplayName = "Tyrant T-1104")
};

// Variantes de zombie especial que arma el patron Builder
UENUM(BlueprintType)
enum class ENexusZombieVariant : uint8
{
    Standard    UMETA(DisplayName = "Estandar"),
    Brawler     UMETA(DisplayName = "Ataque"),
    Tank        UMETA(DisplayName = "Tanque"),
    Speedster   UMETA(DisplayName = "Veloz"),
    Shielded    UMETA(DisplayName = "Escudo")
};