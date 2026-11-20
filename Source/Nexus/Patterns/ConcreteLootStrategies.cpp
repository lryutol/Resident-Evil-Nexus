#include "ConcreteLootStrategies.h"
#include "Engine/World.h"
#include "../Items/NexusPickup.h"

namespace
{
    void SpawnPickup(UWorld* World, const FVector& Location, ENexusPickupType Type)
    {
        if (!World) return;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ANexusPickup* Pickup = World->SpawnActor<ANexusPickup>(
            ANexusPickup::StaticClass(), Location + FVector(0.f, 0.f, 60.f), FRotator::ZeroRotator, Params);

        if (Pickup)
        {
            Pickup->SetPickupType(Type);
            Pickup->SetLifeSpan(40.f); // desaparece si nadie lo toma
        }
    }
}

void UPistolLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    SpawnPickup(World, Location, ENexusPickupType::PistolAmmo);
}

void URifleLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    SpawnPickup(World, Location, ENexusPickupType::RifleAmmo);
}

void UShotgunLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    SpawnPickup(World, Location, ENexusPickupType::ShotgunAmmo);
}

void UHerbLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    SpawnPickup(World, Location, ENexusPickupType::Herb);
}

void UGrenadeLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    SpawnPickup(World, Location, ENexusPickupType::Grenade);
}

void UEmptyLootStrategy::GenerateLoot(UWorld* World, FVector Location)
{
    // No suelta nada
}