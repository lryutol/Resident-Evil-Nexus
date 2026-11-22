#include "NexusZombieAIController.h"
#include "../Characters/NexusZombie.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"

ANexusZombieAIController::ANexusZombieAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    ZombiePawn = nullptr;
    PlayerTarget = nullptr;

    AttackCooldown = 2.0f;
    TimeSinceLastAttack = 0.f;

    DetectionRadius = 800.f;
    ReachingRadius = 160.f;
    AttackRadius = 100.f;

    ChaseSpeed = 50.f;
    ReachSpeed = 20.f;
    bSpeedsReady = false;

    RepathTimer = 0.f;
    RepathInterval = 0.25f;

    CurrentWanderTarget = FVector::ZeroVector;
    WanderTimer = 0.f;
    WanderInterval = 4.0f;
}

void ANexusZombieAIController::BeginPlay()
{
    Super::BeginPlay();
    ZombiePawn = Cast<ANexusZombie>(GetPawn());
    PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ANexusZombieAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ZombiePawn) ZombiePawn = Cast<ANexusZombie>(GetPawn());
    if (!PlayerTarget) PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (!ZombiePawn || !PlayerTarget || ZombiePawn->IsHidden()) return;
    if (ZombiePawn->GetZombieState() == EZombieState::Dead) return;

    // Inconsciente (Tyrant): no se mueve ni ataca
    if (ZombiePawn->GetZombieState() == EZombieState::Downed)
    {
        StopMovement();
        return;
    }

    // Cada enemigo conserva la velocidad que tiene en su Blueprint
    if (!bSpeedsReady)
    {
        ChaseSpeed = FMath::Max(10.f, ZombiePawn->GetCharacterMovement()->MaxWalkSpeed);
        ReachSpeed = ChaseSpeed * 0.4f;
        bSpeedsReady = true;
    }

    TimeSinceLastAttack += DeltaTime;

    RepathTimer += DeltaTime;
    const bool bRepath = (RepathTimer >= RepathInterval);
    if (bRepath) RepathTimer = 0.f;

    const float Distance = FVector::Dist(ZombiePawn->GetActorLocation(), PlayerTarget->GetActorLocation());

    if (Distance <= AttackRadius)
    {
        // 1. ATACAR
        if (ZombiePawn->GetZombieState() != EZombieState::Attacking)
        {
            ZombiePawn->SetZombieState(EZombieState::Attacking);
            ZombiePawn->GetCharacterMovement()->MaxWalkSpeed = 0.f;
            StopMovement();
        }

        if (TimeSinceLastAttack >= AttackCooldown)
        {
            ZombiePawn->PerformAttack();
            TimeSinceLastAttack = 0.f;
        }
    }
    else if (Distance <= ReachingRadius)
    {
        // 2. ESTIRAR BRAZOS
        if (ZombiePawn->GetZombieState() != EZombieState::Reaching)
        {
            ZombiePawn->SetZombieState(EZombieState::Reaching);
            ZombiePawn->PlayReachMontage();
            ZombiePawn->GetCharacterMovement()->MaxWalkSpeed = ReachSpeed;
        }
        if (bRepath) MoveToActor(PlayerTarget, 10.f);
    }
    else if (Distance <= DetectionRadius)
    {
        // 3. PERSEGUIR directo al jugador
        if (ZombiePawn->GetZombieState() != EZombieState::Chase)
        {
            ZombiePawn->SetZombieState(EZombieState::Chase);
            ZombiePawn->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        }
        if (bRepath) MoveToActor(PlayerTarget, 10.f);
    }
    else if (ZombiePawn->HasHuntTarget())
    {
        // 4. CAZAR: el Director indica un punto alrededor del jugador (para rodearlo)
        if (ZombiePawn->GetZombieState() != EZombieState::Chase)
        {
            ZombiePawn->SetZombieState(EZombieState::Chase);
            ZombiePawn->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        }
        if (bRepath) MoveToLocation(ZombiePawn->GetHuntTarget(), 120.f);
    }
    else
    {
        // 5. RONDAR (si no hay Director en el nivel)
        if (ZombiePawn->GetZombieState() != EZombieState::Idle)
        {
            ZombiePawn->SetZombieState(EZombieState::Idle);
            ZombiePawn->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        }

        WanderTimer += DeltaTime;
        if (WanderTimer >= WanderInterval || CurrentWanderTarget.IsZero())
        {
            MoveToRandomPoint();
            WanderTimer = 0.f;
        }
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
            CurrentWanderTarget = RandomLocation.Location;
            MoveToLocation(CurrentWanderTarget, 100.f);
        }
    }
}