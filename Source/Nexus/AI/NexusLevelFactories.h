#pragma once

#include "CoreMinimal.h"
#include "NexusEnemyFactory.h"
#include "NexusLevelFactories.generated.h"

// Fabrica concreta: Fabrica abandonada (especiales: Veloz y Escudo)
UCLASS(Blueprintable)
class NEXUS_API UNexusIndustrialFactory : public UNexusEnemyFactory
{
    GENERATED_BODY()

public:
    virtual ANexusZombie* CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
    virtual ANexusZombie* CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket) override;
    virtual ANexusZombie* CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
};

// Fabrica concreta: Subterraneo (especiales: Tanque y Veloz)
UCLASS(Blueprintable)
class NEXUS_API UNexusUndergroundFactory : public UNexusEnemyFactory
{
    GENERATED_BODY()

public:
    virtual ANexusZombie* CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
    virtual ANexusZombie* CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket) override;
    virtual ANexusZombie* CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
};

// Fabrica concreta: Laboratorios (especiales: Escudo y Ataque)
UCLASS(Blueprintable)
class NEXUS_API UNexusLaboratoryFactory : public UNexusEnemyFactory
{
    GENERATED_BODY()

public:
    virtual ANexusZombie* CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
    virtual ANexusZombie* CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket) override;
    virtual ANexusZombie* CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) override;
};