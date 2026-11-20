#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusSpawnZone.generated.h"

class UBoxComponent;

UCLASS()
class NEXUS_API ANexusSpawnZone : public AActor
{
    GENERATED_BODY()

public:
    ANexusSpawnZone();
    FVector GetRandomPointInZone() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
    UBoxComponent* SpawnBox;
};