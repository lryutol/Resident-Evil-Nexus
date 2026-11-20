#include "NexusKeyItem.h"
#include "../Core/NexusSubsystem.h"
#include "Components/StaticMeshComponent.h"

ANexusKeyItem::ANexusKeyItem()
{
    PrimaryActorTick.bCanEverTick = false;
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
}

void ANexusKeyItem::Interactuar_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("JUGADOR: Ha recogido la llave."));
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this)) Fachada->TriggerBossEvent();
    Destroy();
}