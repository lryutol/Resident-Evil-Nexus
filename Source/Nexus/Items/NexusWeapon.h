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
    void SetEquipped(bool bEquipped);
    bool AddReserveAmmo(int32 Amount);

    float GetWeaponRange() const { return WeaponRange; }
    float GetWeaponDamage() const { return WeaponDamage; }
    int32 GetCurrentAmmo() const { return CurrentAmmo; }
    int32 GetMaxAmmo() const { return MaxAmmo; }
    int32 GetReserveAmmo() const { return ReserveAmmo; }
    ENexusAmmoType GetAmmoType() const { return AmmoType; }

    // OBSERVER (municion)
    void AttachObserver(INexusObserver* Observer);
    void DetachObserver(INexusObserver* Observer);

protected:
    virtual void BeginPlay() override;

    // FACTORY METHOD: cada arma decide que Estrategia de disparo crear
    virtual UObject* CreateFireStrategy();
    void EnsureFireStrategy();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UMeshComponent* WeaponMesh;

    UPROPERTY()
    ANexusCharacter* OwningCharacter;

    // STRATEGY: el objeto se crea en BeginPlay (no como subobjeto por defecto) y se guarda aqui
    UPROPERTY()
    UObject* FireStrategyObject;

    INexusFireStrategy* FireStrategy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon") float WeaponRange;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon") float WeaponDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo") ENexusAmmoType AmmoType;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo") int32 MaxAmmo;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo") int32 CurrentAmmo;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo") int32 ReserveAmmo;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo") int32 MaxReserveAmmo;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform") FVector WeaponOffsetLocation;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform") FRotator WeaponOffsetRotation;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform") FVector WeaponOffsetScale;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound") USoundBase* FireSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound") USoundBase* ReloadSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound") USoundBase* EmptySound;

    void PlayWeaponSound(USoundBase* SoundToPlay);

    TArray<INexusObserver*> AmmoObservers;
    void NotifyAmmoChanged();
};