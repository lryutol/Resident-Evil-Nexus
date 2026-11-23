#include "NexusShotgun.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "../Patterns/ShotgunStrategy.h"

ANexusShotgun::ANexusShotgun()
{
    UStaticMeshComponent* ShotgunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShotgunStaticMesh"));
    RootComponent = ShotgunMesh;
    WeaponMesh = ShotgunMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/modelos_armas/shotgun/shotgun.shotgun"));
    if (MeshAsset.Succeeded())
    {
        ShotgunMesh->SetStaticMesh(MeshAsset.Object);
    }

    ShotgunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ShotgunMesh->SetSimulatePhysics(false);

    AmmoType = ENexusAmmoType::Shotgun;
    WeaponDamage = 15.f;
    MaxAmmo = 8;
    CurrentAmmo = MaxAmmo;
    ReserveAmmo = 16;
    MaxReserveAmmo = 40;
    WeaponRange = 1000.f;

    WeaponOffsetLocation = FVector(-11.f, 2.f, 3.f);
    WeaponOffsetRotation = FRotator(90.f, -90.f, 90.f);
    WeaponOffsetScale = FVector(2.5f, 2.5f, 2.5f);
}

UObject* ANexusShotgun::CreateFireStrategy()
{
    return NewObject<UShotgunStrategy>(this);
}