#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NexusZombieAIController.generated.h"

class ANexusZombie;
class UNexusZombieState;

UCLASS()
class NEXUS_API ANexusZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    ANexusZombieAIController();
    virtual void Tick(float DeltaTime) override;

    void ChangeAIState(UNexusZombieState* NewState);

    ANexusZombie* GetZombie() const;
    ACharacter* GetPlayer() const;

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

    void ChasePlayerSmart(float DeltaTime, float AcceptanceRadius = 20.f);
    void MoveToPointSmart(const FVector& TargetPoint, float DeltaTime, float AcceptanceRadius = 100.f);

    UNexusZombieState* GetWanderState() const { return WanderState; }
    UNexusZombieState* GetHuntState() const { return HuntState; }
    UNexusZombieState* GetChaseState() const { return ChaseState; }
    UNexusZombieState* GetReachState() const { return ReachState; }
    UNexusZombieState* GetAttackState() const { return AttackState; }

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void RefreshReferences();
    void CreateStates();
    void MoveToRandomPoint();
    void ExecuteSmartMovement(const FVector& GoalLocation, AActor* GoalActor, float DeltaTime, float AcceptanceRadius);
    bool FindBestEvadeWaypoint(const FVector& CurrentLoc, const FVector& GoalLocation, FVector& OutWaypoint) const;
    void SteerAlongDirection(const FVector& Direction2D, float DeltaTime);

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

    FVector LastCheckedLocation;
    float StuckTimer;
    float EvadeTimer;
    float EvadeSideSign;
    FVector EvadeWaypoint;
    bool bHasEvadeWaypoint;
};