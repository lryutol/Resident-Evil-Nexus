#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NexusZombieAIController.generated.h"

UCLASS()
class NEXUS_API ANexusZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    ANexusZombieAIController();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    class ANexusZombie* ZombiePawn;

    UPROPERTY()
    class ACharacter* PlayerTarget;

    float AttackCooldown;
    float TimeSinceLastAttack;
    float DetectionRadius;
    float ReachingRadius;
    float AttackRadius;

    // Velocidades propias de cada enemigo (se leen del Blueprint al empezar)
    float ChaseSpeed;
    float ReachSpeed;
    bool bSpeedsReady;

    // Cada cuanto se recalcula la ruta (evita recalcular en cada frame)
    float RepathTimer;
    float RepathInterval;

    FVector CurrentWanderTarget;
    float WanderTimer;
    float WanderInterval;

    void MoveToRandomPoint();
};