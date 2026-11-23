#include "NexusLevelFactories.h"
#include "../Characters/NexusZombie.h"

// ---------- Fabrica abandonada ----------
ANexusZombie* UNexusIndustrialFactory::CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    return SpawnProduct(World, ZombieClass, Location, Rotation);
}

ANexusZombie* UNexusIndustrialFactory::CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket)
{
    ANexusZombie* Product = SpawnProduct(World, SpecialClass, Location, Rotation);
    AssembleVariant(Product, (Bracket % 2 == 1) ? ENexusZombieVariant::Speedster : ENexusZombieVariant::Shielded);
    return Product;
}

ANexusZombie* UNexusIndustrialFactory::CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    ANexusZombie* Boss = SpawnProduct(World, BossClass, Location, Rotation);
    AssembleBoss(Boss);
    return Boss;
}

// ---------- Subterraneo ----------
ANexusZombie* UNexusUndergroundFactory::CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    return SpawnProduct(World, ZombieClass, Location, Rotation);
}

ANexusZombie* UNexusUndergroundFactory::CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket)
{
    ANexusZombie* Product = SpawnProduct(World, SpecialClass, Location, Rotation);
    AssembleVariant(Product, (Bracket % 2 == 1) ? ENexusZombieVariant::Tank : ENexusZombieVariant::Speedster);
    return Product;
}

ANexusZombie* UNexusUndergroundFactory::CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    ANexusZombie* Boss = SpawnProduct(World, BossClass, Location, Rotation);
    AssembleBoss(Boss);
    return Boss;
}

// ---------- Laboratorios ----------
ANexusZombie* UNexusLaboratoryFactory::CreateCommonZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    return SpawnProduct(World, ZombieClass, Location, Rotation);
}

ANexusZombie* UNexusLaboratoryFactory::CreateSpecialZombie(UWorld* World, const FVector& Location, const FRotator& Rotation, int32 Bracket)
{
    ANexusZombie* Product = SpawnProduct(World, SpecialClass, Location, Rotation);
    AssembleVariant(Product, (Bracket % 2 == 1) ? ENexusZombieVariant::Shielded : ENexusZombieVariant::Brawler);
    return Product;
}

ANexusZombie* UNexusLaboratoryFactory::CreateBossZombie(UWorld* World, const FVector& Location, const FRotator& Rotation)
{
    ANexusZombie* Boss = SpawnProduct(World, BossClass, Location, Rotation);
    AssembleBoss(Boss);
    return Boss;
}