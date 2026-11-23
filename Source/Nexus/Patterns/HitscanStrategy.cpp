#include "HitscanStrategy.h"
#include "../Items/NexusWeapon.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UHitscanStrategy::ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character)
{
    if (!Weapon || !Character) return;

    UCameraComponent* Camera = Character->GetFollowCamera();
    UWorld* World = Character->GetWorld();
    if (!Camera || !World)
    {
        UE_LOG(LogTemp, Warning, TEXT("HITSCAN: falta la camara o el mundo."));
        return;
    }

    const FVector Start = Camera->GetComponentLocation();
    const FVector Direction = Camera->GetForwardVector();
    const FVector End = Start + (Direction * Weapon->GetWeaponRange());

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Weapon);
    Params.AddIgnoredActor(Character);

    // Trace por TIPO DE OBJETO: golpea enemigos y paredes sin depender de su configuracion de canales.
    // No incluye PhysicsBody, asi los cadaveres no absorben las balas.
    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
    ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjectParams.AddObjectTypesToQuery(ECC_Destructible);

    FHitResult Hit;
    const bool bHit = World->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, Params);
    const FVector ImpactPoint = bHit ? FVector(Hit.ImpactPoint) : End;

    // Linea amarilla corta: se ve el disparo
    //DrawDebugLine(World, Weapon->GetActorLocation(), ImpactPoint, FColor::Yellow, false, 0.1f, 0, 1.5f);

    if (!bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("HITSCAN: el disparo no impacto nada."));
        return;
    }

    //DrawDebugPoint(World, ImpactPoint, 10.f, FColor::Red, false, 2.f);

    AActor* HitActor = Hit.GetActor();
    UE_LOG(LogTemp, Warning, TEXT("HITSCAN: impacto en %s (hueso: %s)"), *GetNameSafe(HitActor), *Hit.BoneName.ToString());

    if (HitActor)
    {
        UGameplayStatics::ApplyPointDamage(HitActor, Weapon->GetWeaponDamage(), Direction, Hit, Character->GetController(), Weapon, nullptr);
    }
}