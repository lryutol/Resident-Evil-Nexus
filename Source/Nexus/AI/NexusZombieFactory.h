#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h" // <-- NUEVO INCLUDE
#include "NexusEnemyTypes.h"
#include "NexusZombieFactory.generated.h"

class ANexusZombie;

UCLASS(BlueprintType)
class NEXUS_API UNexusZombieFactory : public UDataAsset // <-- AHORA HEREDA DE UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Factory")
    TMap<EZombieType, TSubclassOf<ANexusZombie>> ZombieClasses;

    UFUNCTION(BlueprintCallable, Category = "Factory")
    TSubclassOf<ANexusZombie> GetZombieClass(EZombieType Type) const;
};