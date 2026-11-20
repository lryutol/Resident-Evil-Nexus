#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NexusLootStrategy.generated.h"

UINTERFACE(MinimalAPI)
class UNexusLootStrategy : public UInterface
{
    GENERATED_BODY()
};

class NEXUS_API INexusLootStrategy
{
    GENERATED_BODY()

public:
    // Genera el loot en la ubicación indicada
    virtual void GenerateLoot(UWorld* World, FVector Location) = 0;
};