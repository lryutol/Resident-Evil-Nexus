#include "NexusRifle.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ANexusRifle::ANexusRifle()
{
    UStaticMeshComponent* RifleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleStaticMesh"));
    RootComponent = RifleMesh;
    WeaponMesh = RifleMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/modelos_armas/m4-carbine-rifle/M4_Carbine_by_Umang_Rank.M4_Carbine_by_Umang_Rank"));
    if (MeshAsset.Succeeded())
    {
        RifleMesh->SetStaticMesh(MeshAsset.Object);
    }

    RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RifleMesh->SetSimulatePhysics(false);

    AmmoType = ENexusAmmoType::Rifle;
    WeaponDamage = 35.f;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    ReserveAmmo = 60;
    MaxReserveAmmo = 150;
    WeaponRange = 2500.f;

    WeaponOffsetLocation = FVector(-12.f, -30.f, 3.f);
    WeaponOffsetRotation = FRotator(90.f, -90.f, 90.f);
    WeaponOffsetScale = FVector(4.5f, 4.5f, 4.5f);

    // El rifle usa el HitscanStrategy por defecto
}