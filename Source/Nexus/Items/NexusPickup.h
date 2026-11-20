#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UStaticMesh;

UENUM(BlueprintType)
enum class ENexusPickupType : uint8
{
    PistolAmmo,
    RifleAmmo,
    ShotgunAmmo,
    Herb,
    Grenade,
    Hourglass
};

// Objeto recogible: se toma automaticamente al pasar por encima
UCLASS()
class NEXUS_API ANexusPickup : public AActor
{
    GENERATED_BODY()

public:
    ANexusPickup();

    void SetPickupType(ENexusPickupType NewType);
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* Trigger;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPointLightComponent* Glow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    ENexusPickupType PickupType;

    // Segundos que suma el reloj de arena
    UPROPERTY(EditAnywhere, Category = "Pickup")
    float HourglassSeconds;

    // Cargadores que da un pickup de municion
    UPROPERTY(EditAnywhere, Category = "Pickup")
    int32 AmmoMagazines;

    UPROPERTY()
    UStaticMesh* CubeMesh;

    UPROPERTY()
    UStaticMesh* SphereMesh;

    UPROPERTY()
    UStaticMesh* CylinderMesh;

    void ApplyVisuals();
};