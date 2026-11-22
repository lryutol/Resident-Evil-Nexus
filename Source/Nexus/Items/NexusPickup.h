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

    virtual void Tick(float DeltaTime) override;
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

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float HourglassSeconds;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    int32 AmmoMagazines;

    // <<< NUEVAS VARIABLES PARA GIRO Y SUELO >>>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Animation")
    float RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Animation")
    bool bSnapToGround;

    // Escalas independientes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector PistolAmmoScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector RifleAmmoScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector ShotgunAmmoScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector HerbScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector GrenadeScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Scales")
    FVector HourglassScale;

    // Punteros a los modelos 3D
    UPROPERTY()
    UStaticMesh* PistolAmmoMesh;

    UPROPERTY()
    UStaticMesh* RifleAmmoMesh;

    UPROPERTY()
    UStaticMesh* ShotgunAmmoMesh;

    UPROPERTY()
    UStaticMesh* HerbMesh;

    UPROPERTY()
    UStaticMesh* GrenadeMesh;

    UPROPERTY()
    UStaticMesh* HourglassMesh;

    void ApplyVisuals();
};