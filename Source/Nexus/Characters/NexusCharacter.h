#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    // Arreglo de ítems iniciales (Data Assets)
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<UNexusItemData*> StartingItems;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float BaseWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float AimWalkSpeed;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartAiming();
    void StopAiming();
    void NextWeapon();
    void PreviousWeapon();
    void FireWeapon();
    void ReloadWeapon();
    void ToggleInventory();
};