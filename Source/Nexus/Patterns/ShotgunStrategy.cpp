#include "ShotgunStrategy.h"
#include "../Items/NexusWeapon.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UShotgunStrategy::ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character)
{
    if (!Weapon || !Character) return;

    UCameraComponent* Camera = Character->GetFollowCamera();
    UWorld* World = Character->GetWorld();
    if (!Camera || !World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SHOTGUN: falta la camara o el mundo."));
        return;
    }

    const FVector Start = Camera->GetComponentLocation();
    const FVector Forward = Camera->GetForwardVector();

    int32 PelletsThatHit = 0;

    // Disparamos 5 perdigones en cono
    for (int32 PelletIndex = 0; PelletIndex < 5; ++PelletIndex)
    {
        const FVector SpreadForward = FMath::VRandCone(Forward, FMath::DegreesToRadians(7.0f));
        const FVector End = Start + (SpreadForward * Weapon->GetWeaponRange());

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Weapon);
        Params.AddIgnoredActor(Character);

        const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
        const FVector ImpactPoint = bHit ? FVector(Hit.ImpactPoint) : End;

        DrawDebugLine(World, Weapon->GetActorLocation(), ImpactPoint, FColor::Yellow, false, 0.1f, 0, 1.0f);

        if (!bHit) continue;

        DrawDebugPoint(World, ImpactPoint, 5.f, FColor::Yellow, false, 2.f);

        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            PelletsThatHit++;
            UGameplayStatics::ApplyPointDamage(HitActor, Weapon->GetWeaponDamage(), SpreadForward, Hit, Character->GetController(), Weapon, nullptr);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("SHOTGUN: %d de 5 perdigones impactaron."), PelletsThatHit);
}