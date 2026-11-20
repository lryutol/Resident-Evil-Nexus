#include "NexusZombie.h"
#include "NexusTyrantT1104.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/NexusGameDirector.h"
#include "../Core/NexusSubsystem.h"
#include "../AI/NexusZombieSpawner.h"

ANexusZombie::ANexusZombie()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    GetCharacterMovement()->MaxWalkSpeed = 50.f;

    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;
    bIsDead = false;
    DefaultMultiplier = 1.0f;

    CurrentState = EZombieState::Idle;
    AttackRange = 100.f;
    AttackDamage = 20.f;

    BoneMultipliers.Add(TEXT("head"), 3.0f);
    BoneMultipliers.Add(TEXT("neck_01"), 2.0f);
    BoneMultipliers.Add(TEXT("spine_03"), 1.2f);
    BoneMultipliers.Add(TEXT("spine_02"), 1.0f);

    GameDirector = nullptr;
    OriginSpawner = nullptr;
    bLastHitWasHeadshot = false;
}

void ANexusZombie::BeginPlay()
{
    Super::BeginPlay();

    // La dificultad (Fachada) escala la vida del enemigo
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        MaxHealth *= Fachada->GetDifficultyParams().EnemyHealthMultiplier;
        CurrentHealth = MaxHealth;
    }

    GameDirector = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ANexusGameDirector::StaticClass()));
}

float ANexusZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;

    FName HitBone = NAME_None;
    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        HitBone = PointEvent->HitInfo.BoneName;
    }

    bLastHitWasHeadshot = (HitBone == FName(TEXT("head")));

    float Multiplier = DefaultMultiplier;
    if (BoneMultipliers.Contains(HitBone)) Multiplier = BoneMultipliers[HitBone];

    float FinalDamage = DamageAmount * Multiplier;
    float ActualDamage = Super::TakeDamage(FinalDamage, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: Impacto en '%s' (x%.1f). Dano: %.1f. Salud: %.1f"), *HitBone.ToString(), Multiplier, ActualDamage, CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    else
    {
        if (HitMontage) PlayAnimMontage(HitMontage);
    }
    return ActualDamage;
}

void ANexusZombie::PerformAttack()
{
    if (bIsDead) return;

    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);
        UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: GARRAZO!"));
    }

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Player && FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= AttackRange + 50.f)
    {
        UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
    }
}

void ANexusZombie::PlayReachMontage()
{
    if (ReachMontage)
    {
        PlayAnimMontage(ReachMontage);
    }
}

void ANexusZombie::Die()
{
    if (bIsDead) return;

    bIsDead = true;
    SetZombieState(EZombieState::Dead);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (DeathMontage) PlayAnimMontage(DeathMontage);

    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);
    SetLifeSpan(5.0f);

    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: Zombie ha muerto."));

    // Avisamos al spawner de origen (para saber cuando se limpio la oleada)
    if (OriginSpawner)
    {
        OriginSpawner->NotifyEnemyDied();
    }

    // Notificamos al Director (Patron Observer)
    if (GameDirector)
    {
        GameDirector->NotifyEnemyKilled(this, bLastHitWasHeadshot);

        // Si el que murio es el Tyrant, el escenario termina
        if (Cast<ANexusTyrantT1104>(this))
        {
            GameDirector->NotifyBossDefeated();
        }
    }
}