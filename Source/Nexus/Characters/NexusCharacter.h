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

    // Cura al jugador (hierbas)
    void Heal(float Amount);

    // --- PATRON OBSERVER (Manual) ---
    void AttachObserver(INexusObserver* Observer);
    void DetachObserver(INexusObserver* Observer);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    void Die();
    void ShowGameOverScreen();

    float MaxHealth;
    float CurrentHealth;

    TArray<INexusObserver*> HealthObservers;
    void NotifyHealthChanged();

    FTimerHandle GameOverTimerHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<UNexusItemData*> StartingItems;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float BaseWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float AimWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed;

    bool bSprintHeld;
    void UpdateMoveSpeed();

    // --- INPUTS ---
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