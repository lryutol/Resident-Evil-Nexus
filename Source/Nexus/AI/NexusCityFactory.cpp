#include "NexusCityFactory.h"
#include "../Characters/NexusZombie.h"

ANexusZombie* UNexusCityFactory::CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    return SpawnProduct(World, ZombieClass, Location, Rotation);
}

ANexusZombie* UNexusCityFactory::CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket)
{
    ANexusZombie* Product = SpawnProduct(World, SpecialClass, Location, Rotation);
    AssembleVariant(Product, (Bracket % 2 == 1) ? ENexusZombieVariant::Brawler : ENexusZombieVariant::Tank);
    return Product;
}

ANexusZombie* UNexusCityFactory::CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    ANexusZombie* Boss = SpawnProduct(World, BossClass, Location, Rotation);
    AssembleBoss(Boss);
    return Boss;
}