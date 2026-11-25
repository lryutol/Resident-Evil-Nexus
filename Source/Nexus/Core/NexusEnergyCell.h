#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusEnergyCell.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UParticleSystem;
class USoundBase;
class ANexusGameDirector;

// Celda de energia del nivel final: se destruye a disparos
UCLASS()
class NEXUS_API ANexusEnergyCell : public AActor
{
    GENERATED_BODY()

public:
    ANexusEnergyCell();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;
    void TriggerDestroyExplosion();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPointLightComponent* Glow;

    UPROPERTY(EditAnywhere, Category = "Cell")
    float MaxHealth;

    // Explosion visual sin dano al destruirse la celda
    UPROPERTY(EditAnywhere, Category = "Cell|FX")
    UParticleSystem* ExplosionEffect;

    // Escala rapida de la explosion visual
    UPROPERTY(EditAnywhere, Category = "Cell|FX")
    FVector ExplosionScale;

    // Sonido opcional de la explosion
    UPROPERTY(EditAnywhere, Category = "Cell|FX")
    USoundBase* ExplosionSound;

    float CurrentHealth;
    bool bDestroyed;

    UPROPERTY()
    ANexusGameDirector* Director;
};