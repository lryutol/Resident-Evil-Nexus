#pragma once
#include "CoreMinimal.h"
#include "NexusEnemyFactory.h"
#include "NexusCityFactory.generated.h"

UCLASS()
class NEXUS_API UNexusCityFactory : public UNexusEnemyFactory
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, Category = "Enemies") TSubclassOf<ANexusZombie> ZombieClass;
    UPROPERTY(EditDefaultsOnly, Category = "Enemies") TSubclassOf<ANexusZombie> SpecialClass;

    virtual ANexusZombie* CreateEnemy(UWorld* World, FVector SpawnLocation, FRotator SpawnRotation, EZombieType Type) override;
};