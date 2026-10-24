// Source/Nexus/Core/Interactuable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactuable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractuable : public UInterface
{
    GENERATED_BODY()
};

class NEXUS_API IInteractuable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interactuar(AActor* Interactor);

    virtual void Interactuar_Implementation(AActor* Interactor);
};