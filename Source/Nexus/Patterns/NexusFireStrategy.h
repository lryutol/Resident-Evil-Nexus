#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NexusFireStrategy.generated.h"

class ANexusWeapon;
class ANexusCharacter;

UINTERFACE(MinimalAPI)
class UNexusFireStrategy : public UInterface
{
    GENERATED_BODY()
};

class NEXUS_API INexusFireStrategy
{
    GENERATED_BODY()

public:
    virtual void ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character) = 0;
};