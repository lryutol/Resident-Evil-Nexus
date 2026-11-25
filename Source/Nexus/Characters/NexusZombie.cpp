#include "NexusZombie.h"
#include "NexusTyrantT1104.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/NexusGameDirector.h"
#include "../Core/NexusSubsystem.h"

ANexusZombie::ANexusZombie()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCharacterMovement()->MaxWalkSpeed = 150.f;

    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;
    bIsDead = false;
    DefaultMultiplier = 1.0f;
    HeadMultiplier = 3.0f;
    ArmorReduction = 0.f;
    ShieldHealth = 0.f;

    CurrentState = EZombieState::Idle;
    AttackRange = 100.f;
    AttackDamage = 20.f;

    BoneMultipliers.Add(TEXT("neck_01"), 2.0f);
    BoneMultipliers.Add(TEXT("spine_03"), 1.2f);
    BoneMultipliers.Add(TEXT("spine_02"), 1.0f);

    AttackMontage = nullptr;
    AltAttackMontage = nullptr;
    ReachMontage = nullptr;
    DeathMontage = nullptr;
    HitMontage = nullptr;
    AttackSound = nullptr;
    HitSound = nullptr;
    DeathSound = nullptr;

    GameDirector = nullptr;
    bLastHitWasHeadshot = false;
    LastHitSoundTime = -10.f;
    HuntTargetLocation = FVector::ZeroVector;
    bHasHuntTarget = false;

    VariantGlow = CreateDefaultSubobject<UPointLightComponent>(TEXT("VariantGlow"));
    VariantGlow->SetupAttachment(GetCapsuleComponent());
    VariantGlow->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
    VariantGlow->SetIntensity(0.f);
    VariantGlow->SetCastShadows(false);
    VariantGlow->SetVisibility(false);
}

void ANexusZombie::BeginPlay()
{
    Super::BeginPlay();

    ConfigureHitCollision();

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        MaxHealth *= Fachada->GetDifficultyParams().EnemyHealthMultiplier;
        CurrentHealth = MaxHealth;
    }

    GameDirector = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ANexusGameDirector::StaticClass()));
    if (GameDirector)
    {
        GameDirector->RegisterEnemy(this);
    }
}

void ANexusZombie::ConfigureHitCollision()
{
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
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

void ANexusZombie::ApplyHealthMultiplier(float Multiplier)
{
    MaxHealth *= Multiplier;
    CurrentHealth = MaxHealth;
}

void ANexusZombie::ApplyMoveSpeedMultiplier(float Multiplier)
{
    GetCharacterMovement()->MaxWalkSpeed *= Multiplier;
}

void ANexusZombie::ApplyAttackMultipliers(float DamageMultiplier, float RangeMultiplier)
{
    AttackDamage *= DamageMultiplier;
    AttackRange *= RangeMultiplier;
}

void ANexusZombie::ApplyArmor(float DamageReduction)
{
    ArmorReduction = FMath::Clamp(DamageReduction, 0.f, 0.9f);
}

void ANexusZombie::ApplyShield(float ShieldPoints)
{
    ShieldHealth = FMath::Max(0.f, ShieldPoints);
}

void ANexusZombie::ApplyBodyScale(float NewScale)
{
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    const float OldHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

    SetActorScale3D(FVector(NewScale));

    const float NewHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
    AddActorWorldOffset(FVector(0.f, 0.f, FMath::Max(0.f, NewHalfHeight - OldHalfHeight)));
}

void ANexusZombie::ApplyGlow(const FLinearColor& GlowColor)
{
    if (!VariantGlow) return;

    VariantGlow->SetLightColor(GlowColor);
    VariantGlow->SetIntensity(4000.f);
    VariantGlow->SetAttenuationRadius(350.f);
    VariantGlow->SetVisibility(true);
}

float ANexusZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;

    FName HitBone = NAME_None;
    bool bFrontalHit = false;

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        HitBone = PointEvent->HitInfo.BoneName;

        if (HitBone == NAME_None && GetMesh())
        {
            const FVector ImpactLocation = PointEvent->HitInfo.ImpactPoint;
            HitBone = GetMesh()->FindClosestBone(ImpactLocation);
        }

        const FVector ShotDir = PointEvent->ShotDirection;
        bFrontalHit = FVector::DotProduct(GetActorForwardVector(), -ShotDir) > 0.2f;
    }

    const bool bHeadHit = (HitBone != NAME_None) && HitBone.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
    bLastHitWasHeadshot = bHeadHit;

    float Multiplier = DefaultMultiplier;
    if (BoneMultipliers.Contains(HitBone)) Multiplier = BoneMultipliers[HitBone];
    if (bHeadHit) Multiplier = FMath::Max(Multiplier, HeadMultiplier);

    float FinalDamage = DamageAmount * Multiplier;

    if (ShieldHealth > 0.f && bFrontalHit && !bHeadHit)
    {
        const float Absorbed = FMath::Min(ShieldHealth, FinalDamage);
        ShieldHealth -= Absorbed;
        FinalDamage -= Absorbed;
    }

    FinalDamage *= (1.f - ArmorReduction);

    const float ActualDamage = Super::TakeDamage(FinalDamage, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: Impacto en '%s' (x%.1f). Dano: %.1f. Salud: %.1f. Escudo: %.1f"), *HitBone.ToString(), Multiplier, ActualDamage, CurrentHealth, ShieldHealth);

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

    UAnimMontage* Chosen = AttackMontage;
    if (AltAttackMontage && FMath::FRand() < 0.35f)
    {
        Chosen = AltAttackMontage;
    }
    if (Chosen) PlayAnimMontage(Chosen);
    PlayZombieSound(AttackSound);

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Player)
    {
        const float RadiiSum = GetCapsuleComponent()->GetScaledCapsuleRadius() + Player->GetCapsuleComponent()->GetScaledCapsuleRadius();
        const float SurfaceDistance = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation()) - RadiiSum;

        if (SurfaceDistance <= AttackRange)
        {
            UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
        }
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
    ClearHuntTarget();
    SetZombieState(EZombieState::Dead);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (AController* MyController = GetController())
    {
        MyController->StopMovement();
    }
    PlayZombieSound(DeathSound);

    if (DeathMontage)
    {
        GetCharacterMovement()->DisableMovement();
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        const float MontageLength = PlayAnimMontage(DeathMontage);
        SetLifeSpan(FMath::Max(MontageLength, 1.f) + 4.0f);
    }
    else
    {
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
        GetMesh()->SetSimulatePhysics(true);
        SetLifeSpan(5.0f);
    }

    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: %s ha muerto."), *GetName());

    if (GameDirector)
    {
        GameDirector->NotifyEnemyKilled(this, bLastHitWasHeadshot);

        if (Cast<ANexusTyrantT1104>(this))
        {
            GameDirector->NotifyBossDefeated();
        }
    }
}