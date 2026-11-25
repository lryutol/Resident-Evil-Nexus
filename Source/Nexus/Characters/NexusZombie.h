#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusZombie.generated.h"

class ANexusGameDirector;
class UPointLightComponent;

// PATRON STATE: estados visibles del zombie (los usan la IA, las animaciones y el Director)
UENUM(BlueprintType)
enum class EZombieState : uint8
{
    Idle,
    Chase,
    Reaching,
    Attacking,
    Dead,
    Downed
};

UCLASS()
class NEXUS_API ANexusZombie : public ACharacter
{
    GENERATED_BODY()

public:
    ANexusZombie();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    void PerformAttack();
    void PlayReachMontage();

    EZombieState GetZombieState() const { return CurrentState; }
    void SetZombieState(EZombieState NewState) { CurrentState = NewState; }
    float GetAttackRange() const { return AttackRange; }

    void SetHuntTarget(const FVector& NewTarget) { HuntTargetLocation = NewTarget; bHasHuntTarget = true; }
    void ClearHuntTarget() { HuntTargetLocation = FVector::ZeroVector; bHasHuntTarget = false; }
    bool HasHuntTarget() const { return bHasHuntTarget; }
    FVector GetHuntTarget() const { return HuntTargetLocation; }

    // API que usa el patron Builder
    virtual void ApplyHealthMultiplier(float Multiplier);
    void ApplyMoveSpeedMultiplier(float Multiplier);
    void ApplyAttackMultipliers(float DamageMultiplier, float RangeMultiplier);
    void ApplyArmor(float DamageReduction);
    void ApplyShield(float ShieldPoints);
    void ApplyBodyScale(float NewScale);
    void ApplyGlow(const FLinearColor& GlowColor);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Zones")
    TMap<FName, float> BoneMultipliers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Zones")
    float DefaultMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Zones")
    float HeadMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Animation") class UAnimMontage* AttackMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") class UAnimMontage* AltAttackMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") class UAnimMontage* ReachMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") class UAnimMontage* DeathMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") class UAnimMontage* HitMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Sound") class USoundBase* AttackSound;
    UPROPERTY(EditDefaultsOnly, Category = "Sound") class USoundBase* HitSound;
    UPROPERTY(EditDefaultsOnly, Category = "Sound") class USoundBase* DeathSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EZombieState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackDamage;

    UPROPERTY(VisibleAnywhere, Category = "Variant")
    UPointLightComponent* VariantGlow;

    UPROPERTY()
    ANexusGameDirector* GameDirector;

    float ArmorReduction;
    float ShieldHealth;
    bool bLastHitWasHeadshot;
    float LastHitSoundTime;
    FVector HuntTargetLocation;
    bool bHasHuntTarget;

    void ConfigureHitCollision();
    void PlayZombieSound(class USoundBase* SoundToPlay);

    virtual void Die();
};