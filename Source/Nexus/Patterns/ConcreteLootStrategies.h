#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusLootStrategy.h"
#include "ConcreteLootStrategies.generated.h"

// Cada clase es una ESTRATEGIA de loot distinta (Patron Strategy)

UCLASS()
class NEXUS_API UPistolLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};

UCLASS()
class NEXUS_API URifleLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};

UCLASS()
class NEXUS_API UShotgunLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};

UCLASS()
class NEXUS_API UHerbLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};

UCLASS()
class NEXUS_API UGrenadeLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};

UCLASS()
class NEXUS_API UEmptyLootStrategy : public UObject, public INexusLootStrategy
{
    GENERATED_BODY()
public:
    virtual void GenerateLoot(UWorld* World, FVector Location) override;
};