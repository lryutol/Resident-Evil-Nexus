#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusGrenade.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundBase;
class UParticleSystem;

UCLASS()
class NEXUS_API ANexusGrenade : public AActor
{
    GENERATED_BODY()

public:
    ANexusGrenade();

    // Getter para que el personaje pueda asignarle velocidad al lanzarla
    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* GrenadeMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    // Variables de la explosión
    UPROPERTY(EditDefaultsOnly, Category = "Explosion")
    float ExplosionDelay;

    UPROPERTY(EditDefaultsOnly, Category = "Explosion")
    float DamageRadius;

    UPROPERTY(EditDefaultsOnly, Category = "Explosion")
    float DamageAmount;

    // --- NUEVOS EFECTOS ---
    // Sonido de la explosión
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    USoundBase* ExplosionSound;

    // Efecto visual (Particle System)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* ExplosionParticle;

    FTimerHandle ExplosionTimer;

    UFUNCTION()
    void Explode();
};