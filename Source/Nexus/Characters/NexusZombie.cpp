#include "NexusZombie.h"
#include "NexusTyrantT1104.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundBase.h"
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
    HeadMultiplier = 3.0f;

    CurrentState = EZombieState::Idle;
    AttackRange = 100.f;
    AttackDamage = 20.f;

    BoneMultipliers.Add(TEXT("neck_01"), 2.0f);
    BoneMultipliers.Add(TEXT("spine_03"), 1.2f);
    BoneMultipliers.Add(TEXT("spine_02"), 1.0f);

    AttackSound = nullptr;
    HitSound = nullptr;
    DeathSound = nullptr;

    GameDirector = nullptr;
    OriginSpawner = nullptr;
    bLastHitWasHeadshot = false;
    LastHitSoundTime = -10.f;

    HuntTargetLocation = FVector::ZeroVector;
    bHasHuntTarget = false;
}

void ANexusZombie::BeginPlay()
{
    Super::BeginPlay();

    ConfigureHitCollision();

    // La dificultad (Fachada) escala la vida del enemigo
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        MaxHealth *= Fachada->GetDifficultyParams().EnemyHealthMultiplier;
        CurrentHealth = MaxHealth;
    }

    GameDirector = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ANexusGameDirector::StaticClass()));
    if (GameDirector)
    {
        // El Director conoce a todos los enemigos vivos para guiarlos hacia el jugador
        GameDirector->RegisterEnemy(this);
    }
}

void ANexusZombie::ConfigureHitCollision()
{
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }

    USkeletalMeshComponent* SkelMesh = GetMesh();
    if (SkelMesh)
    {
        SkelMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    }
}

void ANexusZombie::PlayZombieSound(USoundBase* SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
    }
}

float ANexusZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;

    FName HitBone = NAME_None;
    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        HitBone = PointEvent->HitInfo.BoneName;

        // Si el disparo golpeo la capsula (sin hueso), buscamos el hueso mas cercano al impacto
        if (HitBone == NAME_None && GetMesh())
        {
            const FVector ImpactLocation = PointEvent->HitInfo.ImpactPoint;
            HitBone = GetMesh()->FindClosestBone(ImpactLocation);
        }
    }

    const bool bHeadHit = (HitBone != NAME_None) && HitBone.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
    bLastHitWasHeadshot = bHeadHit;

    float Multiplier = DefaultMultiplier;
    if (BoneMultipliers.Contains(HitBone)) Multiplier = BoneMultipliers[HitBone];
    if (bHeadHit) Multiplier = FMath::Max(Multiplier, HeadMultiplier);

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

        const float NowTime = GetWorld()->GetTimeSeconds();
        if (NowTime - LastHitSoundTime > 0.15f)
        {
            LastHitSoundTime = NowTime;
            PlayZombieSound(HitSound);
        }
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
    PlayZombieSound(AttackSound);

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
    bHasHuntTarget = false;
    SetZombieState(EZombieState::Dead);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (DeathMontage) PlayAnimMontage(DeathMontage);
    PlayZombieSound(DeathSound);

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