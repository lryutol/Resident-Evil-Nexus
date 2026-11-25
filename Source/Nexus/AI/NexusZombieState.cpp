#include "NexusZombieState.h"
#include "NexusZombieAIController.h"
#include "../Characters/NexusZombie.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
    ANexusZombie* SafeZombie(ANexusZombieAIController* Brain)
    {
        return IsValid(Brain) ? Brain->GetZombie() : nullptr;
    }

    UCharacterMovementComponent* SafeMove(ANexusZombieAIController* Brain)
    {
        ANexusZombie* Z = SafeZombie(Brain);
        return IsValid(Z) ? Z->GetCharacterMovement() : nullptr;
    }
}

void UNexusWanderState::OnEnter(ANexusZombieAIController* Brain)
{
    ANexusZombie* Z = SafeZombie(Brain);
    UCharacterMovementComponent* M = SafeMove(Brain);
    if (Z) Z->SetZombieState(EZombieState::Idle);
    if (M) M->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusWanderState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    ANexusZombie* Z = SafeZombie(Brain);
    if (!Z || !IsValid(Brain->GetChaseState())) return;

    if (Brain->GetSurfaceDistance() <= Brain->GetDetectionRadius())
    {
        Brain->ChangeAIState(Brain->GetChaseState());
        return;
    }
    if (Z->HasHuntTarget() && IsValid(Brain->GetHuntState()))
    {
        Brain->ChangeAIState(Brain->GetHuntState());
        return;
    }
    Brain->TickWander(DeltaTime);
}

void UNexusHuntState::OnEnter(ANexusZombieAIController* Brain)
{
    ANexusZombie* Z = SafeZombie(Brain);
    UCharacterMovementComponent* M = SafeMove(Brain);
    if (Z) Z->SetZombieState(EZombieState::Chase);
    if (M) M->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusHuntState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    ANexusZombie* Z = SafeZombie(Brain);
    if (!Z) return;

    if (Brain->GetSurfaceDistance() <= Brain->GetDetectionRadius() && IsValid(Brain->GetChaseState()))
    {
        Brain->ChangeAIState(Brain->GetChaseState());
        return;
    }
    if (!Z->HasHuntTarget() && IsValid(Brain->GetWanderState()))
    {
        Brain->ChangeAIState(Brain->GetWanderState());
        return;
    }
    Brain->MoveToPointSmart(Z->GetHuntTarget(), DeltaTime, 120.f);
}

void UNexusChaseState::OnEnter(ANexusZombieAIController* Brain)
{
    ANexusZombie* Z = SafeZombie(Brain);
    UCharacterMovementComponent* M = SafeMove(Brain);
    if (Z) Z->SetZombieState(EZombieState::Chase);
    if (M) M->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusChaseState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    if (!SafeZombie(Brain)) return;

    const float Distance = Brain->GetSurfaceDistance();

    if (Distance <= Brain->GetReachThreshold() && IsValid(Brain->GetReachState()))
    {
        Brain->ChangeAIState(Brain->GetReachState());
        return;
    }
    if (Distance > Brain->GetDetectionRadius() * 1.25f)
    {
        ANexusZombie* Z = SafeZombie(Brain);
        UNexusZombieState* NextState = (Z && Z->HasHuntTarget()) ? Brain->GetHuntState() : Brain->GetWanderState();
        if (IsValid(NextState))
        {
            Brain->ChangeAIState(NextState);
        }
        return;
    }
    Brain->ChasePlayerSmart(DeltaTime, 15.f);
}

void UNexusReachState::OnEnter(ANexusZombieAIController* Brain)
{
    ANexusZombie* Z = SafeZombie(Brain);
    UCharacterMovementComponent* M = SafeMove(Brain);
    if (Z)
    {
        Z->SetZombieState(EZombieState::Reaching);
        Z->PlayReachMontage();
    }
    if (M) M->MaxWalkSpeed = Brain->GetReachSpeed();
}

void UNexusReachState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    if (!SafeZombie(Brain)) return;

    const float Distance = Brain->GetSurfaceDistance();

    if (Distance <= Brain->GetAttackThreshold() && IsValid(Brain->GetAttackState()))
    {
        Brain->ChangeAIState(Brain->GetAttackState());
        return;
    }
    if (Distance > Brain->GetReachThreshold() * 1.35f && IsValid(Brain->GetChaseState()))
    {
        Brain->ChangeAIState(Brain->GetChaseState());
        return;
    }
    Brain->ChasePlayerSmart(DeltaTime, 10.f);
}

void UNexusAttackState::OnEnter(ANexusZombieAIController* Brain)
{
    ANexusZombie* Z = SafeZombie(Brain);
    UCharacterMovementComponent* M = SafeMove(Brain);
    if (Z) Z->SetZombieState(EZombieState::Attacking);
    if (M) M->MaxWalkSpeed = 0.f;
    if (IsValid(Brain)) Brain->StopMovement();
}

void UNexusAttackState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    ANexusZombie* Z = SafeZombie(Brain);
    if (!Z) return;

    if (Brain->GetSurfaceDistance() > Brain->GetAttackThreshold() * 1.35f && IsValid(Brain->GetReachState()))
    {
        Brain->ChangeAIState(Brain->GetReachState());
        return;
    }

    if (IsValid(Brain->GetPlayer()))
    {
        const FVector Dir = (Brain->GetPlayer()->GetActorLocation() - Z->GetActorLocation()).GetSafeNormal2D();
        if (!Dir.IsNearlyZero())
        {
            Z->SetActorRotation(FMath::RInterpTo(Z->GetActorRotation(), FRotator(0.f, Dir.Rotation().Yaw, 0.f), DeltaTime, 10.f));
        }
    }

    if (Brain->CanAttackNow())
    {
        Z->PerformAttack();
        Brain->ResetAttackTimer();
    }
}

void UNexusDownedState::OnEnter(ANexusZombieAIController* Brain)
{
    if (IsValid(Brain))
    {
        Brain->StopMovement();
    }
}