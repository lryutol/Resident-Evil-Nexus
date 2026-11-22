#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusEnemyTypes.h"
#include "NexusEnemyFactory.generated.h"

class ANexusZombie;

UCLASS(Abstract, Blueprintable, BlueprintType)
class NEXUS_API UNexusEnemyFactory : public UObject
{
    GENERATED_BODY()

public:
    // Factory Method estricto
    virtual ANexusZombie* CreateEnemy(UWorld* World, FVector SpawnLocation, FRotator SpawnRotation, EZombieType Type) { return nullptr; }
};