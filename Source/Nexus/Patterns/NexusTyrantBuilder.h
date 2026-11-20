#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusTyrantBuilder.generated.h"

class ANexusTyrantT1104;

UCLASS()
class NEXUS_API UNexusTyrantBuilder : public UObject
{
    GENERATED_BODY()

public:
    void BuildBaseStats(ANexusTyrantT1104* Tyrant);
    void BuildArmor(ANexusTyrantT1104* Tyrant);
    void BuildClaws(ANexusTyrantT1104* Tyrant);
};