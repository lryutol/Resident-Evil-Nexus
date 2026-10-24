// Source/Nexus/Core/Interactuable.cpp
#include "Interactuable.h"

void IInteractuable::Interactuar_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("IInteractuable: Interactuar_Implementation llamado en la clase base."));
}