// Source/Nexus/Core/NexusSubsystem.cpp
#include "NexusSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UNexusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Warning, TEXT("NEXUS: NexusSubsystem inicializado correctamente."));
}

void UNexusSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Warning, TEXT("NEXUS: NexusSubsystem destruido."));
    Super::Deinitialize();
}

UNexusSubsystem* UNexusSubsystem::Get(const UObject* WorldContextObject)
{
    if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        return GameInstance->GetSubsystem<UNexusSubsystem>();
    }
    return nullptr;
}

void UNexusSubsystem::LogSystemStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("NEXUS: El sistema central está activo."));
}