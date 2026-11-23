#include "NexusPickupCreators.h"
#include "Engine/World.h"

ANexusPickup* UNexusPickupCreator::SpawnPickupActor(UWorld* World, const FVector& Location, ENexusPickupType PickupKind, float LifeSeconds) const
{
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ANexusPickup* Pickup = World->SpawnActor<ANexusPickup>(
        ANexusPickup::StaticClass(), Location + FVector(0.f, 0.f, 60.f), FRotator::ZeroRotator, Params);

    if (Pickup)
    {
        Pickup->SetPickupType(PickupKind);
        if (LifeSeconds > 0.f) { Pickup->SetLifeSpan(LifeSeconds); }
    }
    return Pickup;
}

ANexusPickup* UNexusAmmoPickupCreator::CreatePickup(UWorld* World, const FVector& Location)
{
    ENexusPickupType Kind = ENexusPickupType::PistolAmmo;
    if (AmmoKind == ENexusAmmoType::Rifle) { Kind = ENexusPickupType::RifleAmmo; }
    if (AmmoKind == ENexusAmmoType::Shotgun) { Kind = ENexusPickupType::ShotgunAmmo; }
    return SpawnPickupActor(World, Location, Kind, 40.f);
}

ANexusPickup* UNexusHerbPickupCreator::CreatePickup(UWorld* World, const FVector& Location)
{
    return SpawnPickupActor(World, Location, ENexusPickupType::Herb, 40.f);
}

ANexusPickup* UNexusGrenadePickupCreator::CreatePickup(UWorld* World, const FVector& Location)
{
    return SpawnPickupActor(World, Location, ENexusPickupType::Grenade, 40.f);
}

ANexusPickup* UNexusHourglassPickupCreator::CreatePickup(UWorld* World, const FVector& Location)
{
    return SpawnPickupActor(World, Location, ENexusPickupType::Hourglass, 0.f);
}