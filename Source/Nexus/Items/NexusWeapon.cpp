#include "NexusWeapon.h"
#include "DrawDebugHelpers.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h" 

ANexusWeapon::ANexusWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    WeaponMesh = nullptr;
    OwningCharacter = nullptr;
    WeaponRange = 1500.f;
    WeaponDamage = 20.f;
    MaxAmmo = 12;
    CurrentAmmo = MaxAmmo;

    WeaponOffsetLocation = FVector(0.f, 0.f, 0.f);
    WeaponOffsetRotation = FRotator(0.f, 0.f, 0.f);
    WeaponOffsetScale = FVector(1.0f, 1.0f, 1.0f);
}

void ANexusWeapon::AttachToCharacter(ANexusCharacter* Character)
{
    OwningCharacter = Character;
    if (Character && WeaponMesh)
    {
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh->SetSimulatePhysics(false);
        WeaponMesh->SetGenerateOverlapEvents(false);

        // Anclamos al hueso RightHand IGNORANDO su escala (el 100x de Mixamo)
        WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));

        // Aplicamos los valores exactos que configuraste en el socket
        WeaponMesh->SetRelativeLocation(WeaponOffsetLocation);
        WeaponMesh->SetRelativeRotation(WeaponOffsetRotation);
        WeaponMesh->SetRelativeScale3D(WeaponOffsetScale);
    }
}

void ANexusWeapon::FireWeapon()
{
    if (CurrentAmmo <= 0) return;
    CurrentAmmo--;

    if (!OwningCharacter) return;

    FVector Start = OwningCharacter->GetFollowCamera()->GetComponentLocation();
    FVector End = Start + (OwningCharacter->GetFollowCamera()->GetForwardVector() * WeaponRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(OwningCharacter);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Red, false, 2.f);
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            AController* InstigatorController = OwningCharacter->GetController();
            UGameplayStatics::ApplyPointDamage(HitActor, WeaponDamage, (End - Start).GetSafeNormal(), Hit, InstigatorController, this, nullptr);
        }
    }
}

void ANexusWeapon::Reload()
{
    if (CurrentAmmo < MaxAmmo) CurrentAmmo = MaxAmmo;
}