#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusEnemyTypes.h"
#include "NexusEnemyFactory.generated.h"

class ANexusZombie;

// ABSTRACT FACTORY: cada fabrica concreta produce la FAMILIA de enemigos de un nivel.
// FACTORY METHOD: CreateEnemy decide que producto de la familia se instancia.
UCLASS(Abstract, Blueprintable, BlueprintType)
class NEXUS_API UNexusEnemyFactory : public UObject
{
    GENERATED_BODY()

public:
    UNexusEnemyFactory();

    // Factory Method
    virtual ANexusZombie* CreateEnemy(UWorld* World, FVector SpawnLocation, FRotator SpawnRotation, EZombieType Type);

    // Productos de la familia (los implementa cada fabrica concreta)
    virtual ANexusZombie* CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) { return nullptr; }
    virtual ANexusZombie* CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket) { return nullptr; }
    virtual ANexusZombie* CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation) { return nullptr; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Enemies") TSubclassOf<ANexusZombie> ZombieClass;
    UPROPERTY(EditDefaultsOnly, Category = "Enemies") TSubclassOf<ANexusZombie> SpecialClass;
    UPROPERTY(EditDefaultsOnly, Category = "Enemies") TSubclassOf<ANexusZombie> BossClass;

    // Crea el actor apoyado sobre el suelo (el punto recibido es el suelo)
    ANexusZombie* SpawnProduct(UWorld* World, TSubclassOf<ANexusZombie> ProductClass, const FVector& Location, const FRotator& Rotation);

    // Pasan el producto por el Builder
    void AssembleVariant(ANexusZombie* Product, ENexusZombieVariant Variant);
    void AssembleBoss(ANexusZombie* Boss);
};