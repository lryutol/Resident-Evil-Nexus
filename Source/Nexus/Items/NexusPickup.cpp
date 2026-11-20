#include "NexusPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "InventoryComponent.h"
#include "../Characters/NexusCharacter.h"
#include "../Core/NexusGameDirector.h"

ANexusPickup::ANexusPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupType = ENexusPickupType::Herb;
    HourglassSeconds = 30.f;
    AmmoMagazines = 1;

    Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
    Trigger->InitSphereRadius(90.f);
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Trigger->SetGenerateOverlapEvents(true);
    RootComponent = Trigger;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
    Glow->SetupAttachment(RootComponent);
    Glow->SetIntensity(2500.f);
    Glow->SetAttenuationRadius(250.f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    CubeMesh = CubeAsset.Succeeded() ? CubeAsset.Object : nullptr;
    SphereMesh = SphereAsset.Succeeded() ? SphereAsset.Object : nullptr;
    CylinderMesh = CylinderAsset.Succeeded() ? CylinderAsset.Object : nullptr;
}

void ANexusPickup::BeginPlay()
{
    Super::BeginPlay();
    ApplyVisuals();
}

void ANexusPickup::SetPickupType(ENexusPickupType NewType)
{
    PickupType = NewType;
    ApplyVisuals();
}

void ANexusPickup::ApplyVisuals()
{
    UStaticMesh* ChosenMesh = CubeMesh;
    FLinearColor Color = FLinearColor::White;
    FVector Scale(0.25f, 0.25f, 0.25f);

    switch (PickupType)
    {
    case ENexusPickupType::PistolAmmo:  ChosenMesh = CubeMesh;     Color = FLinearColor(1.f, 0.9f, 0.1f); break;  // amarillo
    case ENexusPickupType::RifleAmmo:   ChosenMesh = CubeMesh;     Color = FLinearColor(1.f, 0.4f, 0.0f); Scale = FVector(0.3f, 0.2f, 0.2f); break;  // naranja
    case ENexusPickupType::ShotgunAmmo: ChosenMesh = CubeMesh;     Color = FLinearColor(1.f, 0.1f, 0.1f); Scale = FVector(0.2f, 0.3f, 0.2f); break;  // rojo
    case ENexusPickupType::Herb:        ChosenMesh = SphereMesh;   Color = FLinearColor(0.1f, 1.f, 0.2f); break;  // verde
    case ENexusPickupType::Grenade:     ChosenMesh = SphereMesh;   Color = FLinearColor(0.7f, 0.2f, 1.f); Scale = FVector(0.2f, 0.2f, 0.2f); break;  // violeta
    case ENexusPickupType::Hourglass:   ChosenMesh = CylinderMesh; Color = FLinearColor(0.2f, 0.6f, 1.f); Scale = FVector(0.2f, 0.2f, 0.4f); break;  // azul
    }

    if (Mesh && ChosenMesh)
    {
        Mesh->SetStaticMesh(ChosenMesh);
        Mesh->SetWorldScale3D(Scale);
    }
    if (Glow)
    {
        Glow->SetLightColor(Color);
    }
}

void ANexusPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    ANexusCharacter* Player = Cast<ANexusCharacter>(OtherActor);
    if (!Player) return;

    UInventoryComponent* Inv = Player->GetInventory();
    bool bConsumed = false;

    switch (PickupType)
    {
    case ENexusPickupType::PistolAmmo:
        bConsumed = Inv && Inv->AddAmmo(ENexusAmmoType::Pistol, AmmoMagazines);
        break;
    case ENexusPickupType::RifleAmmo:
        bConsumed = Inv && Inv->AddAmmo(ENexusAmmoType::Rifle, AmmoMagazines);
        break;
    case ENexusPickupType::ShotgunAmmo:
        bConsumed = Inv && Inv->AddAmmo(ENexusAmmoType::Shotgun, AmmoMagazines);
        break;
    case ENexusPickupType::Herb:
        bConsumed = Inv && Inv->AddHerb(1);
        break;
    case ENexusPickupType::Grenade:
        bConsumed = Inv && Inv->AddGrenade(1);
        break;
    case ENexusPickupType::Hourglass:
    {
        ANexusGameDirector* Dir = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(this, ANexusGameDirector::StaticClass()));
        if (Dir)
        {
            Dir->AddBonusTime(HourglassSeconds);
            bConsumed = true;
        }
        break;
    }
    }

    if (bConsumed)
    {
        Destroy();
    }
}