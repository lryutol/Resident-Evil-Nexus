#include "HitscanStrategy.h"
#include "../Items/NexusWeapon.h"
#include "../Characters/NexusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UHitscanStrategy::ExecuteFire(ANexusWeapon* Weapon, ANexusCharacter* Character)
{
    if (!Weapon || !Character) return;

    FVector Start = Character->GetFollowCamera()->GetComponentLocation();
    FVector End = Start + (Character->GetFollowCamera()->GetForwardVector() * Weapon->GetWeaponRange());

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Weapon);
    Params.AddIgnoredActor(Character);

    if (Character->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        DrawDebugPoint(Character->GetWorld(), Hit.ImpactPoint, 10.f, FColor::Red, false, 2.f);
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            UGameplayStatics::ApplyPointDamage(HitActor, Weapon->GetWeaponDamage(), (End - Start).GetSafeNormal(), Hit, Character->GetController(), Weapon, nullptr);
        }
    }
}