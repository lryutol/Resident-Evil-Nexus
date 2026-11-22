#include "NexusCityFactory.h"
#include "../Characters/NexusZombie.h"

ANexusZombie* UNexusCityFactory::CreateEnemy(UWorld* World, FVector SpawnLocation, FRotator SpawnRotation, EZombieType Type)
{
    TSubclassOf<ANexusZombie> ClassToSpawn = nullptr;
    if (Type == EZombieType::Common && ZombieClass) ClassToSpawn = ZombieClass;
    else if (Type == EZombieType::Special && SpecialClass) ClassToSpawn = SpecialClass;

    if (ClassToSpawn)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        return World->SpawnActor<ANexusZombie>(ClassToSpawn, SpawnLocation, FRotator::ZeroRotator, Params);
    }
    return nullptr;
}