#pragma once

#include "CoreMinimal.h"
#include "NexusZombie.h"
#include "NexusTyrantT1104.generated.h"

class UAnimMontage;
class USoundBase;

// PATRON STATE del jefe: Active (pelea), Downed (inconsciente), Roaring (ruge) y GettingUp (se levanta)
UENUM(BlueprintType)
enum class ETyrantPhase : uint8
{
    Active,
    Downed,
    Roaring,
    GettingUp
};

UCLASS()
class NEXUS_API ANexusTyrantT1104 : public ANexusZombie
{
    GENERATED_BODY()

public:
    ANexusTyrantT1104();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    virtual void ApplyHealthMultiplier(float Multiplier) override;

    ETyrantPhase GetPhase() const { return Phase; }

    // Verdadero solo cuando esta activo y puede moverse/atacar
    bool CanActAndMove() const;

protected:
    virtual void BeginPlay() override;
    virtual void Die() override;

    void EnterDownedPhase();
    void ExitDownedPhase();
    void StartRoar();
    void FinishRoar();
    void HaltAIAndMovement();

    UPROPERTY(EditAnywhere, Category = "Tyrant") float DownedDuration;
    UPROPERTY(EditAnywhere, Category = "Tyrant") float HealthLossPerRevive;
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant") float DefaultRoarDuration;

    // Animaciones de la secuencia: cae -> se levanta -> ruge
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant") UAnimMontage* DownedMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant") UAnimMontage* GetUpMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant") UAnimMontage* RoarMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant") USoundBase* RoarSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tyrant")
    ETyrantPhase Phase;

    float OriginalMaxHealth;
    int32 ReviveCount;
    FTimerHandle DownedTimer;
    FTimerHandle GetUpTimer;
    FTimerHandle RoarTimer;
};