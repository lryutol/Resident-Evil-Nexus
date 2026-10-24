#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusWeapon.generated.h"

class UMeshComponent;
class ANexusCharacter;

UCLASS(Abstract)
class NEXUS_API ANexusWeapon : public AActor
{
    GENERATED_BODY()

public:
    ANexusWeapon();

    virtual void FireWeapon();
    virtual void Reload();
    void AttachToCharacter(ANexusCharacter* Character);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UMeshComponent* WeaponMesh;

    UPROPERTY()
    ANexusCharacter* OwningCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float WeaponRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float WeaponDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 MaxAmmo;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
    int32 CurrentAmmo;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FVector WeaponOffsetLocation;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FRotator WeaponOffsetRotation;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Transform")
    FVector WeaponOffsetScale;
};