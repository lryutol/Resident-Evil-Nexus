#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusZombie.generated.h"

UENUM(BlueprintType)
enum class EZombieState : uint8
{
    Idle,
    Chase,
    Reaching,
    Attacking,
    Dead
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

protected:
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

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* ReachMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* DeathMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* HitMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EZombieState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackDamage;

    void Die();
};