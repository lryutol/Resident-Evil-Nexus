#include "NexusPistol.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ANexusPistol::ANexusPistol()
{
    UStaticMeshComponent* PistolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolStaticMesh"));
    RootComponent = PistolMesh;
    WeaponMesh = PistolMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/modelos_armas/pistol-9mm/9mm_pistol.9mm_pistol"));
    if (MeshAsset.Succeeded())
    {
        PistolMesh->SetStaticMesh(MeshAsset.Object);
    }

    PistolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PistolMesh->SetSimulatePhysics(false);

    AmmoType = ENexusAmmoType::Pistol;
    WeaponDamage = 25.f;
    MaxAmmo = 12;
    CurrentAmmo = MaxAmmo;
    ReserveAmmo = 36;
    MaxReserveAmmo = 96;

    WeaponOffsetLocation = FVector(2.f, -13.f, 3.f);
    WeaponOffsetRotation = FRotator(-180.f, -90.f, 90.f);
    WeaponOffsetScale = FVector(0.015f, 0.015f, 0.015f);

    // La pistola usa el HitscanStrategy por defecto (clase base)
}