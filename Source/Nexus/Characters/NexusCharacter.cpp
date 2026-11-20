#include "NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "../Items/InventoryComponent.h"
#include "../Items/NexusWeapon.h"
#include "../Items/NexusItemData.h"
#include "../Core/NexusPlayerController.h"
#include "../Core/NexusSubsystem.h"
#include "../Core/NexusSettingsSave.h"

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
    CameraBoom->TargetArmLength = 100.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 60.f);
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 15.f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    bIsAiming = false;
    bSprintHeld = false;
    SprintSpeed = 380.f;

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
}

void ANexusCharacter::BeginPlay()
{
    Super::BeginPlay();

    BaseWalkSpeed = 150.f;
    AimWalkSpeed = 100.f;
    UpdateMoveSpeed();

    if (InventoryComponent)
    {
        for (UNexusItemData* Item : StartingItems)
        {
            if (Item) InventoryComponent->AddItem(Item);
        }
    }
}

void ANexusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// --- OBSERVER ---
void ANexusCharacter::AttachObserver(INexusObserver* Observer)
{
    if (Observer && !HealthObservers.Contains(Observer))
    {
        HealthObservers.Add(Observer);
    }
}

void ANexusCharacter::DetachObserver(INexusObserver* Observer)
{
    if (Observer) HealthObservers.Remove(Observer);
}

void ANexusCharacter::NotifyHealthChanged()
{
    for (INexusObserver* Obs : HealthObservers)
    {
        if (Obs) Obs->OnHealthChanged(CurrentHealth, MaxHealth);
    }
}

void ANexusCharacter::Heal(float Amount)
{
    if (CurrentHealth <= 0.0f) return;

    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
    NotifyHealthChanged();
}

float ANexusCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f) return 0.0f;

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        DamageAmount *= Fachada->GetDifficultyParams().EnemyDamageMultiplier;
    }

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        CurrentHealth -= ActualDamage;
        NotifyHealthChanged();

        UE_LOG(LogTemp, Warning, TEXT("JUGADOR: Recibio %.1f de dano. Vida restante: %.1f"), ActualDamage, CurrentHealth);

        if (CurrentHealth <= 0.0f)
        {
            Die();
        }
    }

    return ActualDamage;
}

void ANexusCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("JUGADOR: Ha muerto."));

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->SetGameState(ENexusGameState::GameOver);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ANexusCharacter::ShowGameOverScreen, 2.0f, false);
}

void ANexusCharacter::ShowGameOverScreen()
{
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->ShowGameOver();
    }
}

void ANexusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ANexusCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ANexusCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ANexusCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ANexusCharacter::LookUp);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ANexusCharacter::StartAiming);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &ANexusCharacter::StopAiming);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ANexusCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ANexusCharacter::StopSprint);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANexusCharacter::FireWeapon);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANexusCharacter::ReloadWeapon);

    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ANexusCharacter::NextWeapon);
    PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &ANexusCharacter::PreviousWeapon);
    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ANexusCharacter::ToggleInventory);

    PlayerInputComponent->BindAction("Slot1", IE_Pressed, this, &ANexusCharacter::SelectSlot1);
    PlayerInputComponent->BindAction("Slot2", IE_Pressed, this, &ANexusCharacter::SelectSlot2);
    PlayerInputComponent->BindAction("Slot3", IE_Pressed, this, &ANexusCharacter::UseHerb);
    PlayerInputComponent->BindAction("Slot4", IE_Pressed, this, &ANexusCharacter::UseGrenade);
    PlayerInputComponent->BindAction("UseHerb", IE_Pressed, this, &ANexusCharacter::UseHerb);
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

void ANexusCharacter::Turn(float Value)
{
    float Sens = 1.0f;
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        if (Fachada->GetSettings()) Sens = Fachada->GetSettings()->MouseSensitivity;
    }
    AddControllerYawInput(Value * Sens);
}

void ANexusCharacter::LookUp(float Value)
{
    float Sens = 1.0f;
    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        if (Fachada->GetSettings()) Sens = Fachada->GetSettings()->MouseSensitivity;
    }
    AddControllerPitchInput(Value * Sens);
}

// La velocidad depende del estado: apuntando < caminando < corriendo
void ANexusCharacter::UpdateMoveSpeed()
{
    float Speed = BaseWalkSpeed;
    if (bIsAiming) { Speed = AimWalkSpeed; }
    else if (bSprintHeld) { Speed = SprintSpeed; }

    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ANexusCharacter::StartSprint()
{
    bSprintHeld = true;
    UpdateMoveSpeed();
}

void ANexusCharacter::StopSprint()
{
    bSprintHeld = false;
    UpdateMoveSpeed();
}

void ANexusCharacter::StartAiming()
{
    bIsAiming = true;
    UpdateMoveSpeed();
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
    UpdateMoveSpeed();
    CameraBoom->TargetArmLength = 100.0f;
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

void ANexusCharacter::SelectSlot1()
{
    if (InventoryComponent) InventoryComponent->SelectSlot(1);
}

void ANexusCharacter::SelectSlot2()
{
    if (InventoryComponent) InventoryComponent->SelectSlot(2);
}

void ANexusCharacter::UseHerb()
{
    if (InventoryComponent) InventoryComponent->UseHerb();
}

void ANexusCharacter::UseGrenade()
{
    // Pendiente: se implementa en la ultima entrega (lanzamiento de granada)
    UE_LOG(LogTemp, Warning, TEXT("JUGADOR: Granada aun no implementada."));
}

void ANexusCharacter::FireWeapon()
{
    if (!bIsAiming) return;

    if (InventoryComponent)
    {
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