#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusFireStrategy.h"
#include "ShotgunStrategy.generated.h"

UCLASS()
class NEXUS_API UShotgunStrategy : public UObject, public INexusFireStrategy
{
    GENERATED_BODY()

public:
    virtual void ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character) override;
};