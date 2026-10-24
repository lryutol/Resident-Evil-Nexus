#include "NexusShotgun.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"

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

    WeaponDamage = 15.f;
    MaxAmmo = 8;
    CurrentAmmo = MaxAmmo;
    WeaponRange = 1000.f;

    // --- CONFIGURACIÓN BASADA EN LA PISTOLA ---
    WeaponOffsetLocation = FVector(-11.f, 2.f, 3.f); // Si queda flotando, ajusta el Y o Z
    WeaponOffsetRotation = FRotator(90.f, -90.f, 90.f); // Mantenemos la misma rotación
    WeaponOffsetScale = FVector(2.5f, 2.5f, 2.5f); // Escala calculada (12 / 39)
}

void ANexusShotgun::FireWeapon()
{
    if (CurrentAmmo <= 0) return;
    CurrentAmmo--;

    if (!OwningCharacter) return;

    FVector Start = OwningCharacter->GetFollowCamera()->GetComponentLocation();

    for (int i = 0; i < 5; i++)
    {
        FVector Forward = OwningCharacter->GetFollowCamera()->GetForwardVector();
        Forward = FMath::VRandCone(Forward, FMath::DegreesToRadians(7.0f));

        FVector End = Start + (Forward * WeaponRange);

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        Params.AddIgnoredActor(OwningCharacter);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 5.f, FColor::Yellow, false, 2.f);

            AActor* HitActor = Hit.GetActor();
            if (HitActor)
            {
                AController* InstigatorController = OwningCharacter->GetController();
                UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, (End - Start).GetSafeNormal(), Hit, InstigatorController, this, nullptr);
            }
        }
    }
}