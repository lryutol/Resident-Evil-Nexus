#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NexusZombieAIController.generated.h"

class ANexusZombie;
class UNexusZombieState;

// Contexto del patron State: guarda el estado activo y las medidas que usan los estados
UCLASS()
class NEXUS_API ANexusZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    ANexusZombieAIController();
    virtual void Tick(float DeltaTime) override;

    void ChangeState(UNexusZombieState* NewState);

    ANexusZombie* GetZombie() const { return ZombiePawn; }
    ACharacter* GetPlayer() const { return PlayerTarget; }

    // Distancia horizontal entre las SUPERFICIES de las capsulas (no entre centros)
    float GetSurfaceDistance() const;
    float GetAttackThreshold() const;
    float GetReachThreshold() const;
    float GetDetectionRadius() const { return DetectionRadius; }
    float GetChaseSpeed() const { return ChaseSpeed; }
    float GetReachSpeed() const { return ChaseSpeed * 0.4f; }

    bool ShouldRepath() const { return bRepathNow; }
    bool CanAttackNow() const { return TimeSinceLastAttack >= AttackCooldown; }
    void ResetAttackTimer() { TimeSinceLastAttack = 0.f; }
    void TickWander(float DeltaTime);

    UNexusZombieState* GetWanderState() const { return WanderState; }
    UNexusZombieState* GetHuntState() const { return HuntState; }
    UNexusZombieState* GetChaseState() const { return ChaseState; }
    UNexusZombieState* GetReachState() const { return ReachState; }
    UNexusZombieState* GetAttackState() const { return AttackState; }

protected:
    virtual void BeginPlay() override;

    void CreateStates();
    void MoveToRandomPoint();

    UPROPERTY() ANexusZombie* ZombiePawn;
    UPROPERTY() ACharacter* PlayerTarget;

    UPROPERTY() UNexusZombieState* ActiveState;
    UPROPERTY() UNexusZombieState* WanderState;
    UPROPERTY() UNexusZombieState* HuntState;
    UPROPERTY() UNexusZombieState* ChaseState;
    UPROPERTY() UNexusZombieState* ReachState;
    UPROPERTY() UNexusZombieState* AttackState;
    UPROPERTY() UNexusZombieState* DownedState;

    float AttackCooldown;
    float TimeSinceLastAttack;
    float DetectionRadius;

    float ChaseSpeed;
    bool bSpeedsReady;

    float RepathTimer;
    float RepathInterval;
    bool bRepathNow;

    float WanderTimer;
    float WanderInterval;
    FVector WanderTarget;
};