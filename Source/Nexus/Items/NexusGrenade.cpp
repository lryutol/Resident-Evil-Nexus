#include "NexusGrenade.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"

ANexusGrenade::ANexusGrenade()
{
    PrimaryActorTick.bCanEverTick = false;

    GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
    RootComponent = GrenadeMesh;
    GrenadeMesh->SetCollisionProfileName(TEXT("PhysicsActor")); // Permite que rebote

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.2f;
    ProjectileMovement->Friction = 0.5f;

    // Valores por defecto
    ExplosionDelay = 2.5f;
    DamageRadius = 400.f;
    DamageAmount = 150.f;

    // Inicializamos los efectos a nulo por seguridad
    ExplosionSound = nullptr;
    ExplosionParticle = nullptr;
}

void ANexusGrenade::BeginPlay()
{
    Super::BeginPlay();

    // Inicia el temporizador para explotar
    GetWorldTimerManager().SetTimer(ExplosionTimer, this, &ANexusGrenade::Explode, ExplosionDelay, false);
}

void ANexusGrenade::Explode()
{
    if (!GetWorld()) return;

    UE_LOG(LogTemp, Warning, TEXT("GRANADA: ¡BOOM!"));

    // 1. Aplicamos el daño radial
    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        DamageAmount,
        GetActorLocation(),
        DamageRadius,
        UDamageType::StaticClass(),
        TArray<AActor*>(), // Actores a ignorar
        this,              // DamageCauser
        GetInstigatorController(),
        true
    );

    // 2. Reproducimos el sonido de la explosión
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    }

    // 3. Generamos la animación/efecto visual de la explosión
    if (ExplosionParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation());
    }

    // 4. Destruimos la granada tras explotar
    Destroy();
}