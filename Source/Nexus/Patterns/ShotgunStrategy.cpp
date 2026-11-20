#include "ShotgunStrategy.h"
#include "../Items/NexusWeapon.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UShotgunStrategy::ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character)
{
    if (!Weapon || !Character) return;

    FVector Start = Character->GetFollowCamera()->GetComponentLocation();
    FVector Forward = Character->GetFollowCamera()->GetForwardVector();

    // Disparamos 5 perdigones en cono
    for (int i = 0; i < 5; i++)
    {
        FVector SpreadForward = FMath::VRandCone(Forward, FMath::DegreesToRadians(7.0f));
        FVector End = Start + (SpreadForward * Weapon->GetWeaponRange());

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Weapon);
        Params.AddIgnoredActor(Character);

        if (Character->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            DrawDebugPoint(Character->GetWorld(), Hit.ImpactPoint, 5.f, FColor::Yellow, false, 2.f);

            AActor* HitActor = Hit.GetActor();
            if (HitActor)
            {
                UGameplayStatics::ApplyPointDamage(HitActor, Weapon->GetWeaponDamage(), (End - Start).GetSafeNormal(), Hit, Character->GetController(), Weapon, nullptr);
            }
        }
    }
}