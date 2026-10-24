#pragma once

#include "CoreMinimal.h"
#include "NexusEnemyTypes.generated.h"

UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Common UMETA(DisplayName = "Zombie Común"),
    Fast   UMETA(DisplayName = "Zombie Rápido"),
    Tank   UMETA(DisplayName = "Zombie Tanque")
};