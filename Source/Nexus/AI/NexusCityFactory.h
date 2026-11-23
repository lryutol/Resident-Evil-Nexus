#pragma once

#include "CoreMinimal.h"
#include "NexusEnemyFactory.h"
#include "NexusCityFactory.generated.h"

// Fabrica concreta: familia de la Ciudad en ruinas (especiales: Ataque y Tanque)
UCLASS(Blueprintable)
class NEXUS_API UNexusCityFactory : public UNexusEnemyFactory
{
    GENERATED_BODY()

public:
    virtual ANexusZombie* CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
    virtual ANexusZombie* CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket) override;
    virtual ANexusZombie* CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
};