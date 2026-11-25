#include "NexusZombieSpecial.h"
#include "GameFramework/CharacterMovementComponent.h"

ANexusZombieSpecial::ANexusZombieSpecial()
{
    GetCharacterMovement()->MaxWalkSpeed = 250.f;
    MaxHealth = 1000.f;
    CurrentHealth = MaxHealth;
    AttackDamage = 35.f;
    AttackRange = 85.f;
}