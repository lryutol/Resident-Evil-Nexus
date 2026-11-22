#pragma once

#include "CoreMinimal.h"
#include "NexusZombie.h"
#include "NexusTyrantT1104.generated.h"

class UAnimMontage;
class USoundBase;

// PATRON STATE del jefe: Active (pelea), Downed (inconsciente e inmune), Roaring (ruge al levantarse, inmune)
UENUM(BlueprintType)
enum class ETyrantPhase : uint8
{
    Active,
    Downed,
    Roaring
};

UCLASS()
class NEXUS_API ANexusTyrantT1104 : public ANexusZombie
{
    GENERATED_BODY()

public:
    ANexusTyrantT1104();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    ETyrantPhase GetPhase() const { return Phase; }

protected:
    virtual void BeginPlay() override;
    virtual void Die() override;

    void EnterDownedPhase();
    void ExitDownedPhase();
    void FinishRoar();

    // Segundos que pasa inconsciente
    UPROPERTY(EditAnywhere, Category = "Tyrant")
    float DownedDuration;

    // Vida que pierde en cada levantada (0.05 = 5% de la original)
    UPROPERTY(EditAnywhere, Category = "Tyrant")
    float HealthLossPerRevive;

    // Animacion y sonido de rugido al levantarse (se asignan en BP_Tyrant)
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant")
    UAnimMontage* RoarMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Tyrant")
    USoundBase* RoarSound;

    // Duracion del rugido si no hay montaje asignado
    UPROPERTY(EditDefaultsOnly, Category = "Tyrant")
    float DefaultRoarDuration;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tyrant")
    ETyrantPhase Phase;

    float OriginalMaxHealth;
    int32 ReviveCount;
    FTimerHandle DownedTimer;
    FTimerHandle RoarTimer;
};