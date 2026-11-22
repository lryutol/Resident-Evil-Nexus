#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusZombie.generated.h"

class ANexusGameDirector;
class ANexusZombieSpawner;

// PATRON STATE: estados del zombie. Downed se agrego al final para no mover los valores existentes
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

    // El spawner que creo a este zombie (para avisarle cuando muere)
    void SetOriginSpawner(ANexusZombieSpawner* Spawner) { OriginSpawner = Spawner; }

    // El Director le indica a donde ir para rodear al jugador
    void SetHuntTarget(const FVector& NewTarget) { HuntTargetLocation = NewTarget; bHasHuntTarget = true; }
    bool HasHuntTarget() const { return bHasHuntTarget; }
    FVector GetHuntTarget() const { return HuntTargetLocation; }

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

    // Multiplicador del tiro a la cabeza (cualquier hueso cuyo nombre contenga "head")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Zones")
    float HeadMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* ReachMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* DeathMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    class UAnimMontage* HitMontage;

    // --- SONIDOS (se asignan en el Blueprint del enemigo) ---
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* AttackSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* HitSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* DeathSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    EZombieState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackDamage;

    UPROPERTY()
    ANexusGameDirector* GameDirector;

    UPROPERTY()
    ANexusZombieSpawner* OriginSpawner;

    // True si el ultimo impacto recibido fue en la cabeza
    bool bLastHitWasHeadshot;

    // Evita que 5 perdigones de escopeta suenen 5 veces a la vez
    float LastHitSoundTime;

    // Punto que el Director asigno para rodear al jugador
    FVector HuntTargetLocation;
    bool bHasHuntTarget;

    // Fuerza que la capsula y la malla bloqueen los disparos (canal Visibility)
    void ConfigureHitCollision();

    void PlayZombieSound(class USoundBase* SoundToPlay);

    // Virtual: el Tyrant lo sobrescribe para su logica de inmortalidad
    virtual void Die();
};