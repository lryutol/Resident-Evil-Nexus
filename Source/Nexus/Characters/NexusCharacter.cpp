#include "NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Items/InventoryComponent.h"
#include "../Items/NexusWeapon.h"
#include "../Items/NexusItemData.h"
#include "../Core/NexusPlayerController.h"

ANexusCharacter::ANexusCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 0.f;
    GetCharacterMovement()->AirControl = 0.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 250.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 60.f);
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 15.f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    bIsAiming = false;
}

void ANexusCharacter::BeginPlay()
{
    Super::BeginPlay();

    BaseWalkSpeed = 150.f;
    AimWalkSpeed = 100.f;
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    // Añadimos los ítems configurados en el Blueprint
    if (InventoryComponent)
    {
        for (UNexusItemData* Item : StartingItems)
        {
            if (Item)
            {
                InventoryComponent->AddItem(Item);
            }
        }
    }
}

void ANexusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ANexusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ANexusCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ANexusCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ANexusCharacter::StartAiming);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &ANexusCharacter::StopAiming);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANexusCharacter::FireWeapon);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANexusCharacter::ReloadWeapon);

    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ANexusCharacter::NextWeapon);
    PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &ANexusCharacter::PreviousWeapon);
    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ANexusCharacter::ToggleInventory);
}

void ANexusCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void ANexusCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void ANexusCharacter::StartAiming()
{
    bIsAiming = true;
    GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
    CameraBoom->TargetArmLength = 100.0f;
    CameraBoom->SocketOffset = FVector(0.f, 60.f, 50.f);
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = true;

    if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
    {
        PC->ToggleCrosshair(true);
    }
}

void ANexusCharacter::StopAiming()
{
    bIsAiming = false;
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    CameraBoom->TargetArmLength = 250.0f;
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 60.f);
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
    {
        PC->ToggleCrosshair(false);
    }
}

void ANexusCharacter::NextWeapon()
{
    if (InventoryComponent) InventoryComponent->CycleItem(1);
}

void ANexusCharacter::PreviousWeapon()
{
    if (InventoryComponent) InventoryComponent->CycleItem(-1);
}

void ANexusCharacter::FireWeapon()
{
    if (!bIsAiming) return;

    if (InventoryComponent)
    {
        // Usamos el getter que acabamos de crear en el InventoryComponent
        ANexusWeapon* CurrentWeapon = InventoryComponent->GetEquippedWeapon();
        if (CurrentWeapon)
        {
            CurrentWeapon->FireWeapon();
        }
    }
}

void ANexusCharacter::ReloadWeapon()
{
    if (InventoryComponent)
    {
        ANexusWeapon* CurrentWeapon = InventoryComponent->GetEquippedWeapon();
        if (CurrentWeapon)
        {
            CurrentWeapon->Reload();
        }
    }
}

void ANexusCharacter::ToggleInventory()
{
    if (ANexusPlayerController* PC = Cast<ANexusPlayerController>(GetController()))
    {
        PC->ToggleInventory();
    }
}