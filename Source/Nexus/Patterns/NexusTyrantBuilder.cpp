#include "NexusTyrantBuilder.h"
#include "../Characters/NexusTyrantT1104.h"

void UNexusTyrantBuilder::BuildBaseStats(ANexusTyrantT1104* Tyrant)
{
    if (!Tyrant) return;
    UE_LOG(LogTemp, Warning, TEXT("BUILDER: Ensamblando estadísticas base del Tyrant."));
    // Aquí podrías modificar estadísticas si las tuvieras expuestas
}

void UNexusTyrantBuilder::BuildArmor(ANexusTyrantT1104* Tyrant)
{
    if (!Tyrant) return;
    UE_LOG(LogTemp, Warning, TEXT("BUILDER: Añadiendo armadura al Tyrant."));
}

void UNexusTyrantBuilder::BuildClaws(ANexusTyrantT1104* Tyrant)
{
    if (!Tyrant) return;
    UE_LOG(LogTemp, Warning, TEXT("BUILDER: Añadiendo garras letales al Tyrant."));
}