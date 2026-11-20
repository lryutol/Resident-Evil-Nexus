#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Core/Interactuable.h"
#include "NexusKeyItem.generated.h"

UCLASS()
class NEXUS_API ANexusKeyItem : public AActor, public IInteractuable
{
    GENERATED_BODY()
public:
    ANexusKeyItem();
    virtual void Interactuar_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* MeshComp;
};