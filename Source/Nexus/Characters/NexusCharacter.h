#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Patterns/NexusObserver.h"
#include "NexusCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInventoryComponent;
class ANexusWeapon;
class UNexusItemData;
class UAnimMontage;
class USoundBase;
class ANexusGrenade;

UCLASS(config = Game)
class NEXUS_API ANexusCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANexusCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsAiming;

    UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    UInventoryComponent* GetInventory() const { return InventoryComponent; }

    float GetCurrentHealth() const { return CurrentHealth; }
    float GetMaxHealth() const { return MaxHealth; }
    void Heal(float Amount);

    // Datos para el Animation Blueprint
    UFUNCTION(BlueprintPure, Category = "Animation") bool GetIsSprinting() const;
    UFUNCTION(BlueprintPure, Category = "Animation") int32 GetEquippedSlotForAnim() const;

    // OBSERVER
    void AttachObserver(INexusObserver* Observer);
    void DetachObserver(INexusObserver* Observer);

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void Die();
    void ReactToHit();
    void AfterWeaponChange();
    void ShowGameOverScreen();

    float MaxHealth;
    float CurrentHealth;

    TArray<INexusObserver*> HealthObservers;
    void NotifyHealthChanged();

    FTimerHandle GameOverTimerHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") USpringArmComponent* CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") UCameraComponent* FollowCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory") UInventoryComponent* InventoryComponent;
    UPROPERTY(EditDefaultsOnly, Category = "Combat") TArray<UNexusItemData*> StartingItems;

    // Clase de la granada
    UPROPERTY(EditDefaultsOnly, Category = "Combat") TSubclassOf<ANexusGrenade> GrenadeClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement") float BaseWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement") float AimWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement") float SprintSpeed;

    // Animaciones (montajes)
    UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* HitReactionMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* DeathMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Animation") UAnimMontage* EquipLongGunMontage;

    // Sonidos de Kyle
    UPROPERTY(EditDefaultsOnly, Category = "Sound") USoundBase* HurtSound;
    UPROPERTY(EditDefaultsOnly, Category = "Sound") USoundBase* DeathSound;
    UPROPERTY(EditDefaultsOnly, Category = "Sound") USoundBase* HealSound;

    bool bSprintHeld;
    float LastHurtTime;
    int32 LastEquippedSlot;

    void UpdateMoveSpeed();

    // INPUTS
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartAiming();
    void StopAiming();
    void StartSprint();
    void StopSprint();
    void NextWeapon();
    void PreviousWeapon();
    void FireWeapon();
    void ReloadWeapon();
    void ToggleInventory();
    void SelectSlot1();
    void SelectSlot2();
    void UseHerb();
    void UseGrenade();
};