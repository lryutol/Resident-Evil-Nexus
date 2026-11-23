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
    // Activamos el Tick para que pueda rotar
    PrimaryActorTick.bCanEverTick = true;

    PickupType = ENexusPickupType::Herb;
    HourglassSeconds = 30.f;
    AmmoMagazines = 1;

    // Configuración de giro y ajuste al suelo
    RotationSpeed = 45.f;
    bSnapToGround = true;

    // <<< TUS ESCALAS EXACTAS APLICADAS AQUÍ >>>
    PistolAmmoScale = FVector(1.5f, 1.5f, 1.5f);
    RifleAmmoScale = FVector(1.8f, 1.8f, 1.8f);
    ShotgunAmmoScale = FVector(1.5f, 1.5f, 1.5f);
    HerbScale = FVector(2.0f, 2.0f, 2.0f);
    GrenadeScale = FVector(0.07f, 0.07f, 0.07f);
    HourglassScale = FVector(0.2f, 0.2f, 0.2f);

    Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
    Trigger->InitSphereRadius(90.f);
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Trigger->SetGenerateOverlapEvents(true);
    RootComponent = Trigger;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Bajamos un poco el modelo para que quede centrado dentro de la esfera de recogida
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

    Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
    Glow->SetupAttachment(RootComponent);
    Glow->SetIntensity(1500.f);
    Glow->SetAttenuationRadius(200.f);

    // Carga de los modelos 3D
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PistolAmmoAsset(TEXT("/Game/modelos_varios/9MM_Ammo.9MM_Ammo"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> RifleAmmoAsset(TEXT("/Game/modelos_varios/308Win_Ammo.308Win_Ammo"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ShotgunAmmoAsset(TEXT("/Game/modelos_varios/12x70_Ammo.12x70_Ammo"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> HerbAsset(TEXT("StaticMesh'/Game/modelos_varios/kit/fmedbox.fmedbox'"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> GrenadeAsset(TEXT("/Game/modelos_varios/Granate/Model.Model"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> HourglassAsset(TEXT("/Game/modelos_varios/Hourglass_LowQ.Hourglass_LowQ"));

    PistolAmmoMesh = PistolAmmoAsset.Succeeded() ? PistolAmmoAsset.Object : nullptr;
    RifleAmmoMesh = RifleAmmoAsset.Succeeded() ? RifleAmmoAsset.Object : nullptr;
    ShotgunAmmoMesh = ShotgunAmmoAsset.Succeeded() ? ShotgunAmmoAsset.Object : nullptr;
    HerbMesh = HerbAsset.Succeeded() ? HerbAsset.Object : nullptr;
    GrenadeMesh = GrenadeAsset.Succeeded() ? GrenadeAsset.Object : nullptr;
    HourglassMesh = HourglassAsset.Succeeded() ? HourglassAsset.Object : nullptr;
}

void ANexusPickup::BeginPlay()
{
    Super::BeginPlay();

    // <<< LÓGICA PARA PEGARSE AL SUELO >>>
    if (bSnapToGround)
    {
        FVector Start = GetActorLocation();
        FVector End = Start - FVector(0.f, 0.f, 1000.f);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        // Si detecta suelo debajo...
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
        {
            // Lo colocamos justo encima del suelo
            SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, 45.f));
        }
    }

    ApplyVisuals();
}

void ANexusPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // <<< LÓGICA DE GIRO ELEGANTE >>>
    if (RotationSpeed != 0.f)
    {
        AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));
    }
}

void ANexusPickup::SetPickupType(ENexusPickupType NewType)
{
    PickupType = NewType;
    ApplyVisuals();
}

void ANexusPickup::ApplyVisuals()
{
    UStaticMesh* ChosenMesh = nullptr;
    FLinearColor Color = FLinearColor::White;
    FVector ChosenScale(1.0f, 1.0f, 1.0f);

    switch (PickupType)
    {
    case ENexusPickupType::PistolAmmo:
        ChosenMesh = PistolAmmoMesh;
        Color = FLinearColor(1.f, 0.9f, 0.1f); // Amarillo
        ChosenScale = PistolAmmoScale;
        break;
    case ENexusPickupType::RifleAmmo:
        ChosenMesh = RifleAmmoMesh;
        Color = FLinearColor(1.f, 0.4f, 0.0f); // Naranja
        ChosenScale = RifleAmmoScale;
        break;
    case ENexusPickupType::ShotgunAmmo:
        ChosenMesh = ShotgunAmmoMesh;
        Color = FLinearColor(1.f, 0.1f, 0.1f); // Rojo
        ChosenScale = ShotgunAmmoScale;
        break;
    case ENexusPickupType::Herb:
        ChosenMesh = HerbMesh;
        Color = FLinearColor(0.1f, 1.f, 0.2f); // Verde
        ChosenScale = HerbScale;
        break;
    case ENexusPickupType::Grenade:
        ChosenMesh = GrenadeMesh;
        Color = FLinearColor(0.7f, 0.2f, 1.f); // Violeta
        ChosenScale = GrenadeScale;
        break;
    case ENexusPickupType::Hourglass:
        ChosenMesh = HourglassMesh;
        Color = FLinearColor(0.2f, 0.6f, 1.f); // Azul
        ChosenScale = HourglassScale;
        break;
    }

    if (Mesh && ChosenMesh)
    {
        Mesh->SetStaticMesh(ChosenMesh);
        Mesh->SetWorldScale3D(ChosenScale);
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