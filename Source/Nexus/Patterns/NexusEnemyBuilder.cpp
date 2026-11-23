#include "NexusEnemyBuilder.h"
#include "../Characters/NexusZombie.h"

// ---------- Ataque: golpea fuerte y rapido ----------
void UNexusBrawlerBuilder::BuildBaseStats() { Product->ApplyHealthMultiplier(1.2f); }
void UNexusBrawlerBuilder::BuildAttack() { Product->ApplyAttackMultipliers(1.8f, 1.0f); }
void UNexusBrawlerBuilder::BuildMovement() { Product->ApplyMoveSpeedMultiplier(1.3f); }
void UNexusBrawlerBuilder::BuildAppearance() { Product->ApplyGlow(FLinearColor(1.0f, 0.15f, 0.1f)); }

// ---------- Tanque: lento, enorme y con armadura ----------
void UNexusTankBuilder::BuildBaseStats() { Product->ApplyHealthMultiplier(3.0f); }
void UNexusTankBuilder::BuildArmor() { Product->ApplyArmor(0.35f); }
void UNexusTankBuilder::BuildAttack() { Product->ApplyAttackMultipliers(1.3f, 1.1f); }
void UNexusTankBuilder::BuildMovement() { Product->ApplyMoveSpeedMultiplier(0.6f); }
void UNexusTankBuilder::BuildAppearance() { Product->ApplyBodyScale(1.25f); Product->ApplyGlow(FLinearColor(1.0f, 0.6f, 0.1f)); }

// ---------- Veloz: fragil pero muy rapido ----------
void UNexusSpeedsterBuilder::BuildBaseStats() { Product->ApplyHealthMultiplier(0.6f); }
void UNexusSpeedsterBuilder::BuildAttack() { Product->ApplyAttackMultipliers(0.8f, 1.0f); }
void UNexusSpeedsterBuilder::BuildMovement() { Product->ApplyMoveSpeedMultiplier(2.0f); }
void UNexusSpeedsterBuilder::BuildAppearance() { Product->ApplyBodyScale(0.9f); Product->ApplyGlow(FLinearColor(0.6f, 1.0f, 0.1f)); }

// ---------- Escudo: absorbe los impactos frontales ----------
void UNexusShieldBuilder::BuildArmor() { Product->ApplyShield(200.f); Product->ApplyArmor(0.1f); }
void UNexusShieldBuilder::BuildMovement() { Product->ApplyMoveSpeedMultiplier(0.9f); }
void UNexusShieldBuilder::BuildAppearance() { Product->ApplyGlow(FLinearColor(0.1f, 0.8f, 1.0f)); }

// ---------- Jefe (Tyrant): se arma con armadura propia ----------
void UNexusBossBuilder::BuildArmor() { Product->ApplyArmor(0.2f); }
void UNexusBossBuilder::BuildAppearance() { Product->ApplyGlow(FLinearColor(1.0f, 0.05f, 0.05f)); }

// ---------- Director del Builder ----------
void UNexusEnemyAssembler::Construct(UNexusEnemyBuilder* Builder, ANexusZombie* Target)
{
    if (!Builder || !Target) return;

    Builder->StartBuild(Target);
    Builder->BuildBaseStats();
    Builder->BuildArmor();
    Builder->BuildAttack();
    Builder->BuildMovement();
    Builder->BuildAppearance();

    UE_LOG(LogTemp, Warning, TEXT("BUILDER: %s ensamblado."), *GetNameSafe(Target));
}

void UNexusEnemyAssembler::AssembleVariant(ANexusZombie* Target, ENexusZombieVariant Variant)
{
    UNexusEnemyBuilder* Builder = nullptr;

    switch (Variant)
    {
    case ENexusZombieVariant::Brawler:   Builder = NewObject<UNexusBrawlerBuilder>(this);   break;
    case ENexusZombieVariant::Tank:      Builder = NewObject<UNexusTankBuilder>(this);      break;
    case ENexusZombieVariant::Speedster: Builder = NewObject<UNexusSpeedsterBuilder>(this); break;
    case ENexusZombieVariant::Shielded:  Builder = NewObject<UNexusShieldBuilder>(this);    break;
    default: break;
    }

    Construct(Builder, Target);
}

void UNexusEnemyAssembler::AssembleBoss(ANexusZombie* Target)
{
    Construct(NewObject<UNexusBossBuilder>(this), Target);
}