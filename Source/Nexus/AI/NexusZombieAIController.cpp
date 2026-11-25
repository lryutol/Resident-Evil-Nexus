#include "NexusZombieAIController.h"
#include "NexusZombieState.h"
#include "../Characters/NexusZombie.h"
#include "../Characters/NexusTyrantT1104.h"
#include "GameFramework/Character.h"
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
    DetectionRadius = 2200.f;

    ChaseSpeed = 150.f;
    bSpeedsReady = false;

    RepathTimer = 0.f;
    RepathInterval = 0.2f;
    bRepathNow = false;

    WanderTimer = 0.f;
    WanderInterval = 4.0f;
    WanderTarget = FVector::ZeroVector;

    LastCheckedLocation = FVector::ZeroVector;
    StuckTimer = 0.f;
    EvadeTimer = 0.f;
    EvadeSideSign = 1.f;
    EvadeWaypoint = FVector::ZeroVector;
    bHasEvadeWaypoint = false;
}

ANexusZombie* ANexusZombieAIController::GetZombie() const
{
    return IsValid(ZombiePawn) ? ZombiePawn : nullptr;
}

ACharacter* ANexusZombieAIController::GetPlayer() const
{
    return IsValid(PlayerTarget) ? PlayerTarget : nullptr;
}

void ANexusZombieAIController::BeginPlay()
{
    Super::BeginPlay();
    RefreshReferences();
    CreateStates();
}

void ANexusZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ZombiePawn = Cast<ANexusZombie>(InPawn);
    PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    bSpeedsReady = false;
    bHasEvadeWaypoint = false;
    EvadeTimer = 0.f;
    StuckTimer = 0.f;

    if (IsValid(ZombiePawn))
    {
        ZombiePawn->bUseControllerRotationYaw = false;
        if (UCharacterMovementComponent* MoveComp = ZombiePawn->GetCharacterMovement())
        {
            MoveComp->bOrientRotationToMovement = true;
            MoveComp->RotationRate = FRotator(0.f, 360.f, 0.f);
        }
    }

    CreateStates();
}

void ANexusZombieAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopMovement();
    ActiveState = nullptr;
    WanderState = nullptr;
    HuntState = nullptr;
    ChaseState = nullptr;
    ReachState = nullptr;
    AttackState = nullptr;
    DownedState = nullptr;
    ZombiePawn = nullptr;
    PlayerTarget = nullptr;
    Super::EndPlay(EndPlayReason);
}

void ANexusZombieAIController::RefreshReferences()
{
    if (!IsValid(ZombiePawn))
    {
        ZombiePawn = Cast<ANexusZombie>(GetPawn());
    }
    if (!IsValid(PlayerTarget))
    {
        UWorld* World = GetWorld();
        if (IsValid(World))
        {
            PlayerTarget = UGameplayStatics::GetPlayerCharacter(World, 0);
        }
    }
}

void ANexusZombieAIController::CreateStates()
{
    if (IsValid(WanderState)) return;

    WanderState = NewObject<UNexusWanderState>(this);
    HuntState = NewObject<UNexusHuntState>(this);
    ChaseState = NewObject<UNexusChaseState>(this);
    ReachState = NewObject<UNexusReachState>(this);
    AttackState = NewObject<UNexusAttackState>(this);
    DownedState = NewObject<UNexusDownedState>(this);
}

void ANexusZombieAIController::ChangeAIState(UNexusZombieState* NewState)
{
    if (!IsValid(NewState) || NewState == ActiveState) return;

    if (IsValid(ActiveState))
    {
        ActiveState->OnExit(this);
    }
    ActiveState = NewState;
    ActiveState->OnEnter(this);
}

float ANexusZombieAIController::GetSurfaceDistance() const
{
    if (!IsValid(ZombiePawn) || !IsValid(PlayerTarget)) return 100000.f;

    const UCapsuleComponent* ZombieCap = ZombiePawn->GetCapsuleComponent();
    const UCapsuleComponent* PlayerCap = PlayerTarget->GetCapsuleComponent();
    const float RadiiSum = (ZombieCap ? ZombieCap->GetScaledCapsuleRadius() : 42.f)
        + (PlayerCap ? PlayerCap->GetScaledCapsuleRadius() : 42.f);

    return FVector::Dist2D(ZombiePawn->GetActorLocation(), PlayerTarget->GetActorLocation()) - RadiiSum;
}

float ANexusZombieAIController::GetAttackThreshold() const
{
    return IsValid(ZombiePawn) ? ZombiePawn->GetAttackRange() * 0.85f : 80.f;
}

float ANexusZombieAIController::GetReachThreshold() const
{
    return GetAttackThreshold() + 80.f;
}

void ANexusZombieAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RefreshReferences();
    if (!IsValid(ZombiePawn) || !IsValid(PlayerTarget) || ZombiePawn->IsHidden()) return;

    const EZombieState PawnState = ZombiePawn->GetZombieState();
    if (PawnState == EZombieState::Dead)
    {
        StopMovement();
        return;
    }

    CreateStates();
    if (!IsValid(ChaseState) || !IsValid(WanderState) || !IsValid(DownedState)) return;

    UCharacterMovementComponent* MoveComp = ZombiePawn->GetCharacterMovement();
    if (!IsValid(MoveComp)) return;

    if (!bSpeedsReady)
    {
        ChaseSpeed = FMath::Max(50.f, MoveComp->MaxWalkSpeed);
        if (ZombiePawn->IsA(ANexusTyrantT1104::StaticClass()))
        {
            DetectionRadius = 5000.f;
        }
        LastCheckedLocation = ZombiePawn->GetActorLocation();
        bSpeedsReady = true;
    }

    if (ANexusTyrantT1104* Tyrant = Cast<ANexusTyrantT1104>(ZombiePawn))
    {
        if (!Tyrant->CanActAndMove())
        {
            ChangeAIState(DownedState);
            return;
        }
    }
    else if (PawnState == EZombieState::Downed)
    {
        ChangeAIState(DownedState);
        return;
    }

    if (ActiveState == DownedState || !IsValid(ActiveState))
    {
        ChangeAIState(ChaseState);
    }

    TimeSinceLastAttack += DeltaTime;

    RepathTimer += DeltaTime;
    bRepathNow = (RepathTimer >= RepathInterval);
    if (bRepathNow) RepathTimer = 0.f;

    ActiveState->OnTick(this, DeltaTime);
}

void ANexusZombieAIController::ChasePlayerSmart(float DeltaTime, float AcceptanceRadius)
{
    if (!IsValid(ZombiePawn) || !IsValid(PlayerTarget)) return;
    ExecuteSmartMovement(PlayerTarget->GetActorLocation(), PlayerTarget, DeltaTime, AcceptanceRadius);
}

void ANexusZombieAIController::MoveToPointSmart(const FVector& TargetPoint, float DeltaTime, float AcceptanceRadius)
{
    if (!IsValid(ZombiePawn)) return;
    ExecuteSmartMovement(TargetPoint, nullptr, DeltaTime, AcceptanceRadius);
}

bool ANexusZombieAIController::FindBestEvadeWaypoint(const FVector& CurrentLoc, const FVector& GoalLocation, FVector& OutWaypoint) const
{
    UWorld* World = GetWorld();
    UNavigationSystemV1* NavSys = IsValid(World) ? UNavigationSystemV1::GetCurrent(World) : nullptr;
    if (!NavSys) return false;

    const FVector ToGoal = (GoalLocation - CurrentLoc).GetSafeNormal2D();
    if (ToGoal.IsNearlyZero()) return false;

    const float ProbeRadius = IsValid(ZombiePawn) && ZombiePawn->IsA(ANexusTyrantT1104::StaticClass()) ? 450.f : 320.f;
    const float AnglesDeg[6] = {
        45.f * EvadeSideSign,
        -45.f * EvadeSideSign,
        90.f * EvadeSideSign,
        -90.f * EvadeSideSign,
        135.f * EvadeSideSign,
        -135.f * EvadeSideSign
    };

    for (int32 i = 0; i < 6; ++i)
    {
        const FVector CandidateDir = ToGoal.RotateAngleAxis(AnglesDeg[i], FVector::UpVector).GetSafeNormal2D();
        const FVector CandidatePoint = CurrentLoc + CandidateDir * ProbeRadius;

        FNavLocation Projected;
        if (NavSys->ProjectPointToNavigation(CandidatePoint, Projected, FVector(200.f, 200.f, 300.f)))
        {
            if (FVector::Dist2D(Projected.Location, CurrentLoc) > 80.f)
            {
                OutWaypoint = Projected.Location;
                return true;
            }
        }
    }

    FNavLocation RandomReachable;
    if (NavSys->GetRandomReachablePointInRadius(CurrentLoc, ProbeRadius, RandomReachable))
    {
        OutWaypoint = RandomReachable.Location;
        return true;
    }

    return false;
}

void ANexusZombieAIController::SteerAlongDirection(const FVector& Direction2D, float DeltaTime)
{
    if (!IsValid(ZombiePawn) || Direction2D.IsNearlyZero()) return;

    const FVector SafeDir = Direction2D.GetSafeNormal2D();
    ZombiePawn->AddMovementInput(SafeDir, 1.0f);

    const FRotator DesiredRot = SafeDir.Rotation();
    const FRotator CurrentRot = ZombiePawn->GetActorRotation();
    const FRotator SmoothRot = FMath::RInterpTo(CurrentRot, FRotator(0.f, DesiredRot.Yaw, 0.f), DeltaTime, 8.f);
    ZombiePawn->SetActorRotation(SmoothRot);
}

void ANexusZombieAIController::ExecuteSmartMovement(const FVector& GoalLocation, AActor* GoalActor, float DeltaTime, float AcceptanceRadius)
{
    if (!IsValid(ZombiePawn)) return;

    UCharacterMovementComponent* MoveComp = ZombiePawn->GetCharacterMovement();
    if (!IsValid(MoveComp)) return;

    const FVector CurrentLoc = ZombiePawn->GetActorLocation();

    if (EvadeTimer > 0.f)
    {
        EvadeTimer -= DeltaTime;

        if (bHasEvadeWaypoint && FVector::Dist2D(CurrentLoc, EvadeWaypoint) > 90.f)
        {
            if (bRepathNow)
            {
                MoveToLocation(EvadeWaypoint, 75.f, true, true, false, true);
            }
            SteerAlongDirection(EvadeWaypoint - CurrentLoc, DeltaTime);
        }
        else
        {
            bHasEvadeWaypoint = false;
            EvadeTimer = 0.f;
        }

        LastCheckedLocation = CurrentLoc;
        return;
    }

    if (bRepathNow && MoveComp->MaxWalkSpeed > 1.f)
    {
        const float MovedDist = FVector::Dist2D(CurrentLoc, LastCheckedLocation);
        const float ExpectedMinMove = MoveComp->MaxWalkSpeed * RepathInterval * 0.25f;

        if (MovedDist < FMath::Max(6.f, ExpectedMinMove))
        {
            StuckTimer += RepathInterval;
            if (StuckTimer >= 0.45f)
            {
                EvadeSideSign = -EvadeSideSign;
                StuckTimer = 0.f;
                EvadeTimer = 1.1f;
                StopMovement();

                FVector Waypoint;
                if (FindBestEvadeWaypoint(CurrentLoc, GoalLocation, Waypoint))
                {
                    EvadeWaypoint = Waypoint;
                    bHasEvadeWaypoint = true;
                }
                else
                {
                    bHasEvadeWaypoint = false;
                }

                LastCheckedLocation = CurrentLoc;
                return;
            }
        }
        else
        {
            StuckTimer = 0.f;
        }
        LastCheckedLocation = CurrentLoc;
    }

    EPathFollowingRequestResult::Type MoveResult = EPathFollowingRequestResult::RequestSuccessful;
    if (bRepathNow || GetMoveStatus() != EPathFollowingStatus::Moving)
    {
        MoveResult = IsValid(GoalActor)
            ? MoveToActor(GoalActor, AcceptanceRadius, true, true, false, nullptr, true)
            : MoveToLocation(GoalLocation, AcceptanceRadius, true, true, false, true);
    }

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        SteerAlongDirection(GoalLocation - CurrentLoc, DeltaTime);
    }
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
    if (!IsValid(ZombiePawn)) return;

    UWorld* World = GetWorld();
    UNavigationSystemV1* NavSys = IsValid(World) ? UNavigationSystemV1::GetCurrent(World) : nullptr;
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