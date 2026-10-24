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

    WeaponDamage = 35.f;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    WeaponRange = 2500.f;

    // --- CONFIGURACIÓN LÓGICA BASADA EN LA ESCOPETA Y PISTOLA ---

    // Ubicación: Base de la escopeta, con un pequeño ajuste en X por si es más largo
    WeaponOffsetLocation = FVector(-12.f, -30.f, 3.f);

    // Rotación: Usamos la base de la escopeta (que es un arma larga)
    // NOTA: Si apunta al suelo o al techo, cambia el primer número (90.f) por -90.f o 0.f
    WeaponOffsetRotation = FRotator(90.f, -90.f, 90.f);

    // Escala: Calculada matemáticamente (2.5 * 39 / 18 = 5.41 -> 5.5)
    WeaponOffsetScale = FVector(4.5f, 4.5f, 4.5f);
}