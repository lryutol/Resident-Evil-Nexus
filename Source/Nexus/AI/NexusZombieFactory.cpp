#include "NexusZombieFactory.h"
#include "../Characters/NexusZombie.h" // <-- RUTA CORREGIDA

TSubclassOf<ANexusZombie> UNexusZombieFactory::GetZombieClass(EZombieType Type) const
{
    if (ZombieClasses.Contains(Type))
    {
        return ZombieClasses[Type];
    }

    if (ZombieClasses.Contains(EZombieType::Common))
    {
        return ZombieClasses[EZombieType::Common];
    }
    return nullptr;
}