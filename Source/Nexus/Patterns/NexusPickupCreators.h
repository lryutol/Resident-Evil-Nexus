#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Items/NexusPickup.h"
#include "../Items/NexusWeapon.h"
#include "NexusPickupCreators.generated.h"

// FACTORY METHOD: el creador abstracto declara CreatePickup;
// cada creador concreto decide que tipo de pickup fabrica.
UCLASS(Abstract)
class NEXUS_API UNexusPickupCreator : public UObject
{
    GENERATED_BODY()

public:
    virtual ANexusPickup* CreatePickup(UWorld* World, const FVector& Location) { return nullptr; }

protected:
    ANexusPickup* SpawnPickupActor(UWorld* World, const FVector& Location, ENexusPickupType PickupKind, float LifeSeconds) const;
};

UCLASS()
class NEXUS_API UNexusAmmoPickupCreator : public UNexusPickupCreator
{
    GENERATED_BODY()

public:
    void SetAmmoType(ENexusAmmoType NewAmmoType) { AmmoKind = NewAmmoType; }
    virtual ANexusPickup* CreatePickup(UWorld* World, const FVector& Location) override;

protected:
    ENexusAmmoType AmmoKind = ENexusAmmoType::Pistol;
};

UCLASS()
class NEXUS_API UNexusHerbPickupCreator : public UNexusPickupCreator
{
    GENERATED_BODY()

public:
    virtual ANexusPickup* CreatePickup(UWorld* World, const FVector& Location) override;
};

UCLASS()
class NEXUS_API UNexusGrenadePickupCreator : public UNexusPickupCreator
{
    GENERATED_BODY()

public:
    virtual ANexusPickup* CreatePickup(UWorld* World, const FVector& Location) override;
};

UCLASS()
class NEXUS_API UNexusHourglassPickupCreator : public UNexusPickupCreator
{
    GENERATED_BODY()

public:
    virtual ANexusPickup* CreatePickup(UWorld* World, const FVector& Location) override;
};