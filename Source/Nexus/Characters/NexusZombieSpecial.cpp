#include "NexusZombieSpecial.h"
#include "GameFramework/CharacterMovementComponent.h"

ANexusZombieSpecial::ANexusZombieSpecial()
{
    GetCharacterMovement()->MaxWalkSpeed = 200.f;
    MaxHealth = 200.f;
    CurrentHealth = MaxHealth;
    AttackDamage = 35.f;
    AttackRange = 120.f;
}