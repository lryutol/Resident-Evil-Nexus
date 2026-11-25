#include "NexusTyrantT1104.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "../Core/NexusGameDirector.h"

ANexusTyrantT1104::ANexusTyrantT1104()
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = 300.f;
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.f, 240.f, 0.f);
        MoveComp->bUseRVOAvoidance = true;
    }

    bUseControllerRotationYaw = false;

    MaxHealth = 2000.f;
    CurrentHealth = MaxHealth;
    AttackDamage = 60.f;
    AttackRange = 30.f;

    DownedDuration = 14.f;
    HealthLossPerRevive = 0.05f;
    DefaultRoarDuration = 2.5f;
    DownedMontage = nullptr;
    GetUpMontage = nullptr;
    RoarMontage = nullptr;
    RoarSound = nullptr;

    Phase = ETyrantPhase::Active;
    OriginalMaxHealth = MaxHealth;
    ReviveCount = 0;
}

void ANexusTyrantT1104::BeginPlay()
{
    Super::BeginPlay();

    // Vida original (ya con dificultad y Builder)
    OriginalMaxHealth = MaxHealth;
    Phase = ETyrantPhase::Active;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->SetMovementMode(MOVE_Walking);
    }
}

void ANexusTyrantT1104::ApplyHealthMultiplier(float Multiplier)
{
    Super::ApplyHealthMultiplier(Multiplier);
    OriginalMaxHealth = MaxHealth;
}

bool ANexusTyrantT1104::CanActAndMove() const
{
    return CurrentHealth > 0.f && Phase == ETyrantPhase::Active;
}

float ANexusTyrantT1104::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // Inconsciente, levantandose o rugiendo = inmune
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

    HaltAIAndMovement();
    Super::Die();
}

void ANexusTyrantT1104::HaltAIAndMovement()
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
    }

    if (AController* MyController = GetController())
    {
        MyController->StopMovement();
    }
}

void ANexusTyrantT1104::EnterDownedPhase()
{
    if (Phase != ETyrantPhase::Active) return;

    Phase = ETyrantPhase::Downed;
    SetZombieState(EZombieState::Downed);
    HaltAIAndMovement();

    // En el montaje de caida desactiva "Enable Auto Blend Out" para que se quede en el suelo
    UAnimMontage* FallMontage = DownedMontage ? DownedMontage : DeathMontage;
    if (FallMontage) PlayAnimMontage(FallMontage);

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: inconsciente por %.0f segundos. Celdas restantes: %d"),
        DownedDuration, GameDirector ? GameDirector->GetCellsRemaining() : 0);

    GetWorldTimerManager().ClearTimer(GetUpTimer);
    GetWorldTimerManager().ClearTimer(RoarTimer);
    GetWorldTimerManager().SetTimer(DownedTimer, this, &ANexusTyrantT1104::ExitDownedPhase, DownedDuration, false);
}

void ANexusTyrantT1104::ExitDownedPhase()
{
    ReviveCount++;

    // Cada vez se levanta con 5% menos de su vida original (minimo 10%)
    const float Factor = FMath::Max(0.1f, 1.0f - HealthLossPerRevive * ReviveCount);
    CurrentHealth = OriginalMaxHealth * Factor;

    Phase = ETyrantPhase::GettingUp;
    HaltAIAndMovement();

    float GetUpLength = 0.05f;
    if (GetUpMontage)
    {
        const float Played = PlayAnimMontage(GetUpMontage);
        if (Played > 0.f) GetUpLength = Played;
    }

    UE_LOG(LogTemp, Warning, TEXT("TYRANT: se levanta con %.0f de vida (%.0f%% de la original)."), CurrentHealth, Factor * 100.f);

    GetWorldTimerManager().SetTimer(GetUpTimer, this, &ANexusTyrantT1104::StartRoar, GetUpLength, false);
}

void ANexusTyrantT1104::StartRoar()
{
    Phase = ETyrantPhase::Roaring;
    HaltAIAndMovement();

    float RoarLength = DefaultRoarDuration;
    if (RoarMontage)
    {
        const float Played = PlayAnimMontage(RoarMontage);
        if (Played > 0.f) RoarLength = Played;
    }
    PlayZombieSound(RoarSound);

    GetWorldTimerManager().SetTimer(RoarTimer, this, &ANexusTyrantT1104::FinishRoar, RoarLength, false);
}

void ANexusTyrantT1104::FinishRoar()
{
    Phase = ETyrantPhase::Active;
    SetZombieState(EZombieState::Idle);

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->SetMovementMode(MOVE_Walking);
    }
}