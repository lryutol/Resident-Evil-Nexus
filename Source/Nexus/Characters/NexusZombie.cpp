#include "NexusZombie.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

ANexusZombie::ANexusZombie()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // --- IGNORAR COLISIÓN ENTRE ZOMBIES ---
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // --- VELOCIDAD REDUCIDA PARA UN ZOMBIE LENTO ---
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

    float Multiplier = DefaultMultiplier;
    if (BoneMultipliers.Contains(HitBone)) Multiplier = BoneMultipliers[HitBone];

    float FinalDamage = DamageAmount * Multiplier;
    float ActualDamage = Super::TakeDamage(FinalDamage, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: Impacto en '%s' (x%.1f). Daño: %.1f. Salud: %.1f"), *HitBone.ToString(), Multiplier, ActualDamage, CurrentHealth);

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
        UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: ¡GARRAZO!"));
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
    bIsDead = true;
    SetZombieState(EZombieState::Dead);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (DeathMontage) PlayAnimMontage(DeathMontage);

    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);
    SetLifeSpan(5.0f);
    UE_LOG(LogTemp, Warning, TEXT("NEXUS ZOMBIE: Zombie ha muerto."));
}