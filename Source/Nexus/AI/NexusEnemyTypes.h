#pragma once
#include "CoreMinimal.h"
#include "NexusEnemyTypes.generated.h"

UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Common      UMETA(DisplayName = "Zombie Común"),
    Special     UMETA(DisplayName = "Zombie Especial"),
    BossT1104   UMETA(DisplayName = "Tyrant T-1104")
};