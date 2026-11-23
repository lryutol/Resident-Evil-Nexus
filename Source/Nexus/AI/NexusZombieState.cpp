#include "NexusZombieState.h"
#include "NexusZombieAIController.h"
#include "../Characters/NexusZombie.h"
#include "GameFramework/CharacterMovementComponent.h"

// ---------- Rondar ----------
void UNexusWanderState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->GetZombie()->SetZombieState(EZombieState::Idle);
    Brain->GetZombie()->GetCharacterMovement()->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusWanderState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    if (Brain->GetSurfaceDistance() <= Brain->GetDetectionRadius())
    {
        Brain->ChangeState(Brain->GetChaseState());
        return;
    }
    if (Brain->GetZombie()->HasHuntTarget())
    {
        Brain->ChangeState(Brain->GetHuntState());
        return;
    }
    Brain->TickWander(DeltaTime);
}

// ---------- Cazar (el Director indica un punto alrededor del jugador) ----------
void UNexusHuntState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->GetZombie()->SetZombieState(EZombieState::Chase);
    Brain->GetZombie()->GetCharacterMovement()->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusHuntState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    if (Brain->GetSurfaceDistance() <= Brain->GetDetectionRadius())
    {
        Brain->ChangeState(Brain->GetChaseState());
        return;
    }
    if (!Brain->GetZombie()->HasHuntTarget())
    {
        Brain->ChangeState(Brain->GetWanderState());
        return;
    }
    if (Brain->ShouldRepath())
    {
        Brain->MoveToLocation(Brain->GetZombie()->GetHuntTarget(), 120.f);
    }
}

// ---------- Perseguir al jugador ----------
void UNexusChaseState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->GetZombie()->SetZombieState(EZombieState::Chase);
    Brain->GetZombie()->GetCharacterMovement()->MaxWalkSpeed = Brain->GetChaseSpeed();
}

void UNexusChaseState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    const float Distance = Brain->GetSurfaceDistance();

    if (Distance <= Brain->GetReachThreshold())
    {
        Brain->ChangeState(Brain->GetReachState());
        return;
    }
    if (Distance > Brain->GetDetectionRadius() * 1.2f)
    {
        Brain->ChangeState(Brain->GetZombie()->HasHuntTarget() ? Brain->GetHuntState() : Brain->GetWanderState());
        return;
    }
    if (Brain->ShouldRepath())
    {
        Brain->MoveToActor(Brain->GetPlayer(), 10.f);
    }
}

// ---------- Estirar los brazos ----------
void UNexusReachState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->GetZombie()->SetZombieState(EZombieState::Reaching);
    Brain->GetZombie()->PlayReachMontage();
    Brain->GetZombie()->GetCharacterMovement()->MaxWalkSpeed = Brain->GetReachSpeed();
}

void UNexusReachState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    const float Distance = Brain->GetSurfaceDistance();

    if (Distance <= Brain->GetAttackThreshold())
    {
        Brain->ChangeState(Brain->GetAttackState());
        return;
    }
    if (Distance > Brain->GetReachThreshold() * 1.3f)
    {
        Brain->ChangeState(Brain->GetChaseState());
        return;
    }
    if (Brain->ShouldRepath())
    {
        Brain->MoveToActor(Brain->GetPlayer(), 10.f);
    }
}

// ---------- Atacar ----------
void UNexusAttackState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->GetZombie()->SetZombieState(EZombieState::Attacking);
    Brain->GetZombie()->GetCharacterMovement()->MaxWalkSpeed = 0.f;
    Brain->StopMovement();
}

void UNexusAttackState::OnTick(ANexusZombieAIController* Brain, float DeltaTime)
{
    if (Brain->GetSurfaceDistance() > Brain->GetAttackThreshold() * 1.3f)
    {
        Brain->ChangeState(Brain->GetReachState());
        return;
    }
    if (Brain->CanAttackNow())
    {
        Brain->GetZombie()->PerformAttack();
        Brain->ResetAttackTimer();
    }
}

// ---------- Inconsciente ----------
void UNexusDownedState::OnEnter(ANexusZombieAIController* Brain)
{
    Brain->StopMovement();
}