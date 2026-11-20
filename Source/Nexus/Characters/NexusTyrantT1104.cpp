#include "NexusTyrantT1104.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "../Core/NexusGameDirector.h"

ANexusTyrantT1104::ANexusTyrantT1104()
{
    GetCharacterMovement()->MaxWalkSpeed = 100.f;
    MaxHealth = 2000.f;
    CurrentHealth = MaxHealth;
    AttackDamage = 60.f;
    AttackRange = 200.f;

    DownedDuration = 15.f;
    HealthLossPerRevive = 0.05f;
    Phase = ETyrantPhase::Active;
    OriginalMaxHealth = MaxHealth;
    ReviveCount = 0;
}

void ANexusTyrantT1104::BeginPlay()
{
    Super::BeginPlay();

    // Guardamos la vida original (ya con el multiplicador de dificultad)
    OriginalMaxHealth = MaxHealth;
    Phase = ETyrantPhase::Active;
}

float ANexusTyrantT1104::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // Inconsciente = inmune
    if (Phase == ETyrantPhase::Downed) return 0.f;

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ANexusTyrantT1104::Die()
{
    // Mientras queden celdas de energia no puede morir: queda inconsciente
    if (GameDirector && GameDirector->GetCellsRemaining() > 0)
    {
        EnterDownedPhase();
        return;
    }

    Super::Die();
}

void ANexusTyrantT1104::EnterDownedPhase()
{
    if (Phase == ETyrantPhase::Downed) return;

    Phase = ETyrantPhase::Downed;
    SetZombieState(EZombieState::Downed);

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    if (AController* C = GetController())
    {
        C->StopMovement();
    }

    // Aqui ira la animacion de caida (se ajusta al final)
    if (DeathMontage) PlayAnimMontage(DeathMontage);

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: Inconsciente por %.0f segundos. Celdas restantes: %d"),
        DownedDuration, GameDirector ? GameDirector->GetCellsRemaining() : 0);

    GetWorldTimerManager().SetTimer(DownedTimer, this, &ANexusTyrantT1104::ExitDownedPhase, DownedDuration, false);
}

void ANexusTyrantT1104::ExitDownedPhase()
{
    ReviveCount++;

    // Cada vez se levanta con 5% menos de su vida original (minimo 10%)
    const float Factor = FMath::Max(0.1f, 1.0f - HealthLossPerRevive * ReviveCount);
    CurrentHealth = OriginalMaxHealth * Factor;

    Phase = ETyrantPhase::Active;
    SetZombieState(EZombieState::Idle);

    // Aqui ira la animacion de levantarse
    StopAnimMontage();
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: Se levanta con %.0f de vida (%.0f%% de la original)."), CurrentHealth, Factor * 100.f);
}