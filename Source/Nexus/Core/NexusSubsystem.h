// Source/Nexus/Core/NexusSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NexusSubsystem.generated.h"

UCLASS()
class NEXUS_API UNexusSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    static UNexusSubsystem* Get(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "Nexus|Core")
    void LogSystemStatus() const;
};