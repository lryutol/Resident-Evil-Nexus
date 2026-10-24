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

    class ANexusZombie* ZombiePawn;
    class ACharacter* PlayerTarget;

    float AttackCooldown;
    float TimeSinceLastAttack;
    float DetectionRadius;
    float ReachingRadius;
    float AttackRadius;

    FVector CurrentWanderTarget;
    float WanderTimer;
    float WanderInterval;

    void MoveToRandomPoint();
};