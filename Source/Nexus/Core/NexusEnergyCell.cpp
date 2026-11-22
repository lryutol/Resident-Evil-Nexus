#include "NexusEnergyCell.h"
#include "NexusGameDirector.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ANexusEnergyCell::ANexusEnergyCell()
{
    PrimaryActorTick.bCanEverTick = false;

    MaxHealth = 300.f;
    CurrentHealth = MaxHealth;
    bDestroyed = false;
    Director = nullptr;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("StaticMesh'/Game/modelos_varios/Hourglass_LowQ.Hourglass_LowQ'"));
    if (CylinderAsset.Succeeded())
    {
        Mesh->SetStaticMesh(CylinderAsset.Object);
    }
    Mesh->SetWorldScale3D(FVector(0.8f, 0.8f, 2.5f));
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));

    Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
    Glow->SetupAttachment(RootComponent);
    Glow->SetLightColor(FLinearColor(0.1f, 0.8f, 1.f));
    Glow->SetIntensity(5000.f);
    Glow->SetAttenuationRadius(600.f);
}

void ANexusEnergyCell::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    Director = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(this, ANexusGameDirector::StaticClass()));
    if (Director)
    {
        Director->RegisterCell();
    }
}

float ANexusEnergyCell::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bDestroyed) return 0.f;

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    if (CurrentHealth <= 0.f)
    {
        bDestroyed = true;
        if (Director)
        {
            Director->NotifyCellDestroyed();
        }
        Destroy();
    }

    return ActualDamage;
}