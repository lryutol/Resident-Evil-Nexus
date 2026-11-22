#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Patterns/NexusObserver.h"
#include "../Patterns/NexusFireStrategy.h"
#include "NexusWeapon.generated.h"

class UMeshComponent;
class USoundBase;
class ANexusCharacter;

UENUM(BlueprintType)
enum class ENexusAmmoType : uint8
{
    Pistol,
    Rifle,
    Shotgun
};

UCLASS(Abstract)
class NEXUS_API ANexusWeapon : public AActor
{
    GENERATED_BODY()

public:
    ANexusWeapon();

    virtual void FireWeapon();
    virtual void Reload();
    void AttachToCharacter(ANexusCharacter* Character);

    // Muestra u oculta el arma (solo la equipada es visible)
    void SetEquipped(bool bEquipped);

    // Devuelve false si la reserva ya estaba llena
    bool AddReserveAmmo(int32 Amount);

    float GetWeaponRange() const { return WeaponRange; }
    float GetWeaponDamage() const { return WeaponDamage; }
    int32 GetCurrentAmmo() const { return CurrentAmmo; }
    int32 GetMaxAmmo() const { return MaxAmmo; }
    int32 GetReserveAmmo() const { return ReserveAmmo; }
    ENexusAmmoType GetAmmoType() const { return AmmoType; }

    // --- PATRON OBSERVER (Manual) ---
    void AttachObserver(INexusObserver* Observer);
    void DetachObserver(INexusObserver* Observer);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UMeshComponent* WeaponMesh;

    UPROPERTY()
    ANexusCharacter* OwningCharacter;

    // --- PATRON STRATEGY ---
    UPROPERTY()
    TScriptInterface<INexusFireStrategy> FireStrategy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float WeaponRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float WeaponDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    ENexusAmmoType AmmoType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 MaxAmmo;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 CurrentAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 ReserveAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 MaxReserveAmmo;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FVector WeaponOffsetLocation;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FRotator WeaponOffsetRotation;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FVector WeaponOffsetScale;

    // --- SONIDOS (se asignan en el Blueprint hijo de cada arma) ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
    USoundBase* FireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
    USoundBase* ReloadSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
    USoundBase* EmptySound;

    void PlayWeaponSound(USoundBase* SoundToPlay);

    TArray<INexusObserver*> AmmoObservers;
    void NotifyAmmoChanged();
};