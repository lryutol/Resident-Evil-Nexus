#include "ConcreteLootStrategies.h"
#include "Engine/World.h"
#include "NexusPickupCreators.h"

// Cada ESTRATEGIA de loot usa un CREADOR (Factory Method) para fabricar el pickup

void UPistolLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    UNexusAmmoPickupCreator* Creator = NewObject<UNexusAmmoPickupCreator>(this);
    Creator->SetAmmoType(ENexusAmmoType::Pistol);
    Creator->CreatePickup(World, Location);
}

void URifleLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    UNexusAmmoPickupCreator* Creator = NewObject<UNexusAmmoPickupCreator>(this);
    Creator->SetAmmoType(ENexusAmmoType::Rifle);
    Creator->CreatePickup(World, Location);
}

void UShotgunLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    UNexusAmmoPickupCreator* Creator = NewObject<UNexusAmmoPickupCreator>(this);
    Creator->SetAmmoType(ENexusAmmoType::Shotgun);
    Creator->CreatePickup(World, Location);
}

void UHerbLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    UNexusHerbPickupCreator* Creator = NewObject<UNexusHerbPickupCreator>(this);
    Creator->CreatePickup(World, Location);
}

void UGrenadeLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    UNexusGrenadePickupCreator* Creator = NewObject<UNexusGrenadePickupCreator>(this);
    Creator->CreatePickup(World, Location);
}

void UEmptyLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    // No suelta nada
}