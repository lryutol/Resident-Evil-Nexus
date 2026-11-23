#include "NexusZombieSpawner.h" // SIEMPRE EL PRIMERO
#include "NexusEnemyFactory.h"
#include "NexusCityFactory.h"
#include "../Characters/NexusZombie.h"
#include "../Core/NexusSubsystem.h"
#include "../Core/NexusGameDirector.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

ANexusZombieSpawner::ANexusZombieSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    EnemyFactoryClass = UNexusCityFactory::StaticClass();
    bIsBossSpawner = false;
    SpawnInterval = 1.0f;

    CurrentFactory = nullptr;
    Director = nullptr;
    PendingToSpawn = 0;
    bBossSpawned = false;
}

void ANexusZombieSpawner::BeginPlay()
{
    Super::BeginPlay();

    Director = Cast<ANexusGameDirector>(UGameplayStatics::GetActorOfClass(this, ANexusGameDirector::StaticClass()));
    EnsureFactory();

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->AttachObserver(this);
    }
}

void ANexusZombieSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

    if (UNexusSubsystem* Fachada = UNexusSubsystem::Get(this))
    {
        Fachada->DetachObserver(this);
    }
    Super::EndPlay(EndPlayReason);
}

bool ANexusZombieSpawner::EnsureFactory()
{
    if (!CurrentFactory && EnemyFactoryClass)
    {
        CurrentFactory = NewObject<UNexusEnemyFactory>(this, EnemyFactoryClass);
    }
    return CurrentFactory != nullptr;
}

void ANexusZombieSpawner::OnGameStateChanged(ENexusGameState NewState)
{
    if (NewState == ENexusGameState::BossFight && bIsBossSpawner && !bBossSpawned)
    {
        SpawnBoss();
    }
}

// Busca un punto SOBRE el NavMesh, cerca de una zona y libre de otros enemigos
bool ANexusZombieSpawner::FindSpawnLocation(FVector& OutLocation) const
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys || SpawnZones.Num() == 0) return false;

    for (int32 Attempt = 0; Attempt < 8; ++Attempt)
    {
        ANexusSpawnZone* Zone = SpawnZones[FMath::RandRange(0, SpawnZones.Num() - 1)];
        if (!Zone) continue;

        // Extension vertical corta: evita proyectar a un piso de otro nivel
        FNavLocation NavPoint;
        if (!NavSys->ProjectPointToNavigation(Zone->GetRandomPointInZone(), NavPoint, FVector(80.f, 80.f, 150.f))) continue;

        const FVector Probe = NavPoint.Location + FVector(0.f, 0.f, 100.f);
        if (GetWorld()->OverlapAnyTestByChannel(Probe, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeCapsule(45.f, 90.f))) continue;

        OutLocation = NavPoint.Location;
        return true;
    }
    return false;
}

void ANexusZombieSpawner::StartWave(int32 Count)
{
    if (!EnsureFactory() || SpawnZones.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SPAWNER %s: falta fabrica o zonas de aparicion."), *GetName());
        return;
    }

    PendingToSpawn += FMath::Max(0, Count);

    if (!GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ANexusZombieSpawner::SpawnNext, FMath::Max(0.2f, SpawnInterval), true, 0.0f);
    }
}

void ANexusZombieSpawner::SpawnNext()
{
    if (PendingToSpawn <= 0)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }
    PendingToSpawn--;

    // El Director controla la cuota total del nivel
    if (Director && !Director->TryRegisterSpawn())
    {
        PendingToSpawn = 0;
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    FVector Location;
    if (!FindSpawnLocation(Location))
    {
        UE_LOG(LogTemp, Warning, TEXT("SPAWNER %s: no hay un punto valido sobre el NavMesh en sus zonas."), *GetName());
        if (Director) Director->UnregisterSpawn();
        return;
    }

    ANexusZombie* Enemy = CurrentFactory->CreateEnemy(GetWorld(), Location, FRotator::ZeroRotator, EZombieType::Common);
    if (!Enemy && Director)
    {
        Director->UnregisterSpawn();
    }
}

void ANexusZombieSpawner::SpawnMiniBoss(int32 Bracket)
{
    if (!EnsureFactory()) return;

    FVector Location = GetActorLocation();
    FindSpawnLocation(Location);

    ANexusZombie* MiniBoss = CurrentFactory->CreateSpecialZombie(GetWorld(), Location, FRotator::ZeroRotator, Bracket);
    if (MiniBoss && Director)
    {
        Director->RegisterAlive();
    }
    UE_LOG(LogTemp, Warning, TEXT("SPAWNER %s: Mini-Boss %d %s."), *GetName(), Bracket, MiniBoss ? TEXT("creado") : TEXT("FALLO"));
}

bool ANexusZombieSpawner::SpawnBoss()
{
    if (!EnsureFactory()) return false;

    FVector Location = GetActorLocation();
    FindSpawnLocation(Location);

    ANexusZombie* Boss = CurrentFactory->CreateBossZombie(GetWorld(), Location, FRotator::ZeroRotator);
    if (Boss)
    {
        bBossSpawned = true;
        UE_LOG(LogTemp, Warning, TEXT("SPAWNER %s: el Tyrant T-1104 ha sido ensamblado y creado."), *GetName());
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("SPAWNER %s: NO se pudo crear al Tyrant (revisa BP_Tyrant)."), *GetName());
    return false;
}