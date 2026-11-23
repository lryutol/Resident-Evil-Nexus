#include "NexusEnemyFactory.h"
#include "../Characters/NexusZombie.h"
#include "../Patterns/NexusEnemyBuilder.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

UNexusEnemyFactory::UNexusEnemyFactory()
{
    // El constructor se queda vacío intencionalmente.
    // Las clases (ZombieClass, SpecialClass, BossClass) se asignan 
    // desde el editor en las fábricas hijas (ej. BP_CityFactory) 
    // para evitar el error CDO Constructor.
}

ANexusZombie* UNexusEnemyFactory::CreateEnemy(UWorld* World, FVector SpawnLocation, FRotator SpawnRotation, EZombieType Type)
{
    switch (Type)
    {
    case EZombieType::Common:    return CreateCommonZombie(World, SpawnLocation, SpawnRotation);
    case EZombieType::Special:   return CreateSpecialZombie(World, SpawnLocation, SpawnRotation, 1);
    case EZombieType::BossT1104: return CreateBossZombie(World, SpawnLocation, SpawnRotation);
    default:                     return nullptr;
    }
}

ANexusZombie* UNexusEnemyFactory::SpawnProduct(UWorld* World, TSubclassOf<ANexusZombie> ProductClass, const FVector& Location, const FRotator& Rotation)
{
    if (!World || !ProductClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("FABRICA: no hay clase de enemigo asignada."));
        return nullptr;
    }

    // Subimos el punto la mitad de la capsula para que los pies queden sobre el suelo
    float HalfHeight = 96.f;
    const ANexusZombie* Template = ProductClass->GetDefaultObject<ANexusZombie>();
    if (Template && Template->GetCapsuleComponent())
    {
        HalfHeight = Template->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    return World->SpawnActor<ANexusZombie>(ProductClass, Location + FVector(0.f, 0.f, HalfHeight + 2.f), Rotation, Params);
}

void UNexusEnemyFactory::AssembleVariant(ANexusZombie* Product, ENexusZombieVariant Variant)
{
    if (!Product) return;
    UNexusEnemyAssembler* Assembler = NewObject<UNexusEnemyAssembler>(this);
    Assembler->AssembleVariant(Product, Variant);
}

void UNexusEnemyFactory::AssembleBoss(ANexusZombie* Boss)
{
    if (!Boss) return;
    UNexusEnemyAssembler* Assembler = NewObject<UNexusEnemyAssembler>(this);
    Assembler->AssembleBoss(Boss);
}