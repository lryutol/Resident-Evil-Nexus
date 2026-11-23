#include "NexusZombieAIController.h"
#include "NexusZombieState.h"
#include "../Characters/NexusZombie.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"

ANexusZombieAIController::ANexusZombieAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    ZombiePawn = nullptr;
    PlayerTarget = nullptr;
    ActiveState = nullptr;
    WanderState = nullptr;
    HuntState = nullptr;
    ChaseState = nullptr;
    ReachState = nullptr;
    AttackState = nullptr;
    DownedState = nullptr;

    AttackCooldown = 2.0f;
    TimeSinceLastAttack = 2.0f;
    DetectionRadius = 800.f;

    ChaseSpeed = 50.f;
    bSpeedsReady = false;

    RepathTimer = 0.f;
    RepathInterval = 0.25f;
    bRepathNow = false;

    WanderTimer = 0.f;
    WanderInterval = 4.0f;
    WanderTarget = FVector::ZeroVector;
}

void ANexusZombieAIController::BeginPlay()
{
    Super::BeginPlay();
    ZombiePawn = Cast<ANexusZombie>(GetPawn());
    PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    CreateStates();
}

void ANexusZombieAIController::CreateStates()
{
    if (WanderState) return;

    WanderState = NewObject<UNexusWanderState>(this);
    HuntState = NewObject<UNexusHuntState>(this);
    ChaseState = NewObject<UNexusChaseState>(this);
    ReachState = NewObject<UNexusReachState>(this);
    AttackState = NewObject<UNexusAttackState>(this);
    DownedState = NewObject<UNexusDownedState>(this);
}

void ANexusZombieAIController::ChangeState(UNexusZombieState* NewState)
{
    if (!NewState || NewState == ActiveState) return;

    if (ActiveState) ActiveState->OnExit(this);
    ActiveState = NewState;
    ActiveState->OnEnter(this);
}

float ANexusZombieAIController::GetSurfaceDistance() const
{
    if (!ZombiePawn || !PlayerTarget) return 100000.f;

    const float RadiiSum = ZombiePawn->GetCapsuleComponent()->GetScaledCapsuleRadius()
        + PlayerTarget->GetCapsuleComponent()->GetScaledCapsuleRadius();
    return FVector::Dist2D(ZombiePawn->GetActorLocation(), PlayerTarget->GetActorLocation()) - RadiiSum;
}

float ANexusZombieAIController::GetAttackThreshold() const
{
    return ZombiePawn ? ZombiePawn->GetAttackRange() * 0.8f : 80.f;
}

float ANexusZombieAIController::GetReachThreshold() const
{
    return GetAttackThreshold() + 80.f;
}

void ANexusZombieAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ZombiePawn) ZombiePawn = Cast<ANexusZombie>(GetPawn());
    if (!PlayerTarget) PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!ZombiePawn || !PlayerTarget || ZombiePawn->IsHidden()) return;

    CreateStates();

    const EZombieState PawnState = ZombiePawn->GetZombieState();
    if (PawnState == EZombieState::Dead) return;

    // Cada enemigo conserva la velocidad de su Blueprint (el Builder ya la modifico)
    if (!bSpeedsReady)
    {
        ChaseSpeed = FMath::Max(10.f, ZombiePawn->GetCharacterMovement()->MaxWalkSpeed);
        bSpeedsReady = true;
    }

    // Inconsciente (lo decide el propio enemigo, por ejemplo el Tyrant)
    if (PawnState == EZombieState::Downed)
    {
        ChangeState(DownedState);
        return;
    }
    if (ActiveState == DownedState || !ActiveState)
    {
        ChangeState(WanderState);
    }

    TimeSinceLastAttack += DeltaTime;

    RepathTimer += DeltaTime;
    bRepathNow = (RepathTimer >= RepathInterval);
    if (bRepathNow) RepathTimer = 0.f;

    ActiveState->OnTick(this, DeltaTime);
}

void ANexusZombieAIController::TickWander(float DeltaTime)
{
    WanderTimer += DeltaTime;
    if (WanderTimer >= WanderInterval || WanderTarget.IsZero())
    {
        MoveToRandomPoint();
        WanderTimer = 0.f;
    }
}

void ANexusZombieAIController::MoveToRandomPoint()
{
    if (!ZombiePawn) return;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        FNavLocation RandomLocation;
        if (NavSys->GetRandomReachablePointInRadius(ZombiePawn->GetActorLocation(), 1000.f, RandomLocation))
        {
            WanderTarget = RandomLocation.Location;
            MoveToLocation(WanderTarget, 100.f);
        }
    }
}