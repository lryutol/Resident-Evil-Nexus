#pragma once

#include "CoreMinimal.h"
#include "NexusWeapon.h"
#include "NexusShotgun.generated.h"

UCLASS()
class NEXUS_API ANexusShotgun : public ANexusWeapon
{
    GENERATED_BODY()

public:
    ANexusShotgun();

protected:
    // FACTORY METHOD: la escopeta usa la estrategia de 5 perdigones
    virtual UObject* CreateFireStrategy() override;
};