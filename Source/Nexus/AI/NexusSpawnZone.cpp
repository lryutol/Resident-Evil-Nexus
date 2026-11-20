#include "NexusSpawnZone.h"
#include "Components/BoxComponent.h"

ANexusSpawnZone::ANexusSpawnZone()
{
    PrimaryActorTick.bCanEverTick = false;
    SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
    RootComponent = SpawnBox;
    SpawnBox->SetBoxExtent(FVector(200.f, 200.f, 100.f));
    SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ANexusSpawnZone::GetRandomPointInZone() const
{
    if (!SpawnBox) return GetActorLocation();
    FVector Origin = SpawnBox->GetComponentLocation();
    FVector Extent = SpawnBox->GetScaledBoxExtent();
    FBox Bounds = FBox::BuildAABB(Origin, Extent);
    return FMath::RandPointInBox(Bounds);
}