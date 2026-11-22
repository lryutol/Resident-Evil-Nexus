#include "NexusTyrantT1104.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
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
    RoarMontage = nullptr;
    RoarSound = nullptr;
    DefaultRoarDuration = 2.5f;
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
    // Inconsciente o rugiendo = inmune
    if (Phase != ETyrantPhase::Active) return 0.f;

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
    if (Phase != ETyrantPhase::Active) return;

    Phase = ETyrantPhase::Downed;
    SetZombieState(EZombieState::Downed);

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    if (AController* C = GetController())
    {
        C->StopMovement();
    }

    // Animacion de caida (en el montaje desactiva "Enable Auto Blend Out" para que se quede en el suelo)
    if (DeathMontage) PlayAnimMontage(DeathMontage);

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: Inconsciente por %.0f segundos. Celdas restantes: %d"),
        DownedDuration, GameDirector ? GameDirector->GetCellsRemaining() : 0);

    GetWorldTimerManager().ClearTimer(RoarTimer);
    GetWorldTimerManager().SetTimer(DownedTimer, this, &ANexusTyrantT1104::ExitDownedPhase, DownedDuration, false);
}

void ANexusTyrantT1104::ExitDownedPhase()
{
    ReviveCount++;

    // Cada vez se levanta con 5% menos de su vida original (minimo 10%)
    const float Factor = FMath::Max(0.1f, 1.0f - HealthLossPerRevive * ReviveCount);
    CurrentHealth = OriginalMaxHealth * Factor;

    // Ruge inmovil e inmune; el estado Downed mantiene quieta a la IA
    Phase = ETyrantPhase::Roaring;
    StopAnimMontage();

    float RoarLength = DefaultRoarDuration;
    if (RoarMontage)
    {
        const float PlayedLength = PlayAnimMontage(RoarMontage);
        if (PlayedLength > 0.f)
        {
            RoarLength = PlayedLength;
        }
    }
    PlayZombieSound(RoarSound);

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: Ruge y se levanta con %.0f de vida (%.0f%% de la original)."), CurrentHealth, Factor * 100.f);

    GetWorldTimerManager().SetTimer(RoarTimer, this, &ANexusTyrantT1104::FinishRoar, RoarLength, false);
}

void ANexusTyrantT1104::FinishRoar()
{
    Phase = ETyrantPhase::Active;
    SetZombieState(EZombieState::Idle);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}