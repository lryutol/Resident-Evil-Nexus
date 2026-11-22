#include "NexusWeapon.h"
#include "DrawDebugHelpers.h"
#include "../Characters/NexusCharacter.h"
#include "../Patterns/HitscanStrategy.h" 
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h" 

ANexusWeapon::ANexusWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    WeaponMesh = nullptr;
    OwningCharacter = nullptr;
    WeaponRange = 1500.f;
    WeaponDamage = 20.f;

    AmmoType = ENexusAmmoType::Pistol;
    MaxAmmo = 12;
    CurrentAmmo = MaxAmmo;
    ReserveAmmo = 36;
    MaxReserveAmmo = 96;

    WeaponOffsetLocation = FVector(0.f, 0.f, 0.f);
    WeaponOffsetRotation = FRotator(0.f, 0.f, 0.f);
    WeaponOffsetScale = FVector(1.0f, 1.0f, 1.0f);

    FireSound = nullptr;
    ReloadSound = nullptr;
    EmptySound = nullptr;

    FireStrategy = CreateDefaultSubobject<UHitscanStrategy>(TEXT("HitscanStrategy"));
}

void ANexusWeapon::AttachToCharacter(ANexusCharacter* Character)
{
    OwningCharacter = Character;
    if (Character && WeaponMesh)
    {
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh->SetSimulatePhysics(false);
        WeaponMesh->SetGenerateOverlapEvents(false);

        WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
        WeaponMesh->SetRelativeLocation(WeaponOffsetLocation);
        WeaponMesh->SetRelativeRotation(WeaponOffsetRotation);
        WeaponMesh->SetRelativeScale3D(WeaponOffsetScale);
    }
}

void ANexusWeapon::SetEquipped(bool bEquipped)
{
    SetActorHiddenInGame(!bEquipped);
}

bool ANexusWeapon::AddReserveAmmo(int32 Amount)
{
    if (ReserveAmmo >= MaxReserveAmmo) return false;

    ReserveAmmo = FMath::Min(MaxReserveAmmo, ReserveAmmo + Amount);
    NotifyAmmoChanged();
    return true;
}

void ANexusWeapon::PlayWeaponSound(USoundBase* SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
    }
}

void ANexusWeapon::AttachObserver(INexusObserver* Observer)
{
    if (Observer && !AmmoObservers.Contains(Observer))
    {
        AmmoObservers.Add(Observer);
    }
}

void ANexusWeapon::DetachObserver(INexusObserver* Observer)
{
    if (Observer) AmmoObservers.Remove(Observer);
}

void ANexusWeapon::NotifyAmmoChanged()
{
    for (INexusObserver* Obs : AmmoObservers)
    {
        if (Obs) Obs->OnAmmoChanged(CurrentAmmo, ReserveAmmo);
    }
}

void ANexusWeapon::FireWeapon()
{
    if (CurrentAmmo <= 0)
    {
        PlayWeaponSound(EmptySound); // "click" de arma vacia
        return;
    }

    CurrentAmmo--;
    NotifyAmmoChanged();
    PlayWeaponSound(FireSound);

    if (!OwningCharacter) return;

    if (FireStrategy)
    {
        FireStrategy->ExecuteFire(this, OwningCharacter);
    }
}

void ANexusWeapon::Reload()
{
    if (CurrentAmmo >= MaxAmmo || ReserveAmmo <= 0) return;

    const int32 Taken = FMath::Min(MaxAmmo - CurrentAmmo, ReserveAmmo);
    CurrentAmmo += Taken;
    ReserveAmmo -= Taken;
    NotifyAmmoChanged();
    PlayWeaponSound(ReloadSound);
}