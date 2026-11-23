#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../AI/NexusEnemyTypes.h"
#include "NexusEnemyBuilder.generated.h"

class ANexusZombie;

// BUILDER (abstracto): define los pasos para armar un enemigo
UCLASS(Abstract)
class NEXUS_API UNexusEnemyBuilder : public UObject
{
    GENERATED_BODY()

public:
    void StartBuild(ANexusZombie* Target) { Product = Target; }
    ANexusZombie* GetProduct() const { return Product; }

    virtual void BuildBaseStats() {}
    virtual void BuildArmor() {}
    virtual void BuildAttack() {}
    virtual void BuildMovement() {}
    virtual void BuildAppearance() {}

protected:
    UPROPERTY()
    ANexusZombie* Product = nullptr;
};

// Constructores concretos
UCLASS()
class NEXUS_API UNexusBrawlerBuilder : public UNexusEnemyBuilder
{
    GENERATED_BODY()
public:
    virtual void BuildBaseStats() override;
    virtual void BuildAttack() override;
    virtual void BuildMovement() override;
    virtual void BuildAppearance() override;
};

UCLASS()
class NEXUS_API UNexusTankBuilder : public UNexusEnemyBuilder
{
    GENERATED_BODY()
public:
    virtual void BuildBaseStats() override;
    virtual void BuildArmor() override;
    virtual void BuildAttack() override;
    virtual void BuildMovement() override;
    virtual void BuildAppearance() override;
};

UCLASS()
class NEXUS_API UNexusSpeedsterBuilder : public UNexusEnemyBuilder
{
    GENERATED_BODY()
public:
    virtual void BuildBaseStats() override;
    virtual void BuildAttack() override;
    virtual void BuildMovement() override;
    virtual void BuildAppearance() override;
};

UCLASS()
class NEXUS_API UNexusShieldBuilder : public UNexusEnemyBuilder
{
    GENERATED_BODY()
public:
    virtual void BuildArmor() override;
    virtual void BuildMovement() override;
    virtual void BuildAppearance() override;
};

UCLASS()
class NEXUS_API UNexusBossBuilder : public UNexusEnemyBuilder
{
    GENERATED_BODY()
public:
    virtual void BuildArmor() override;
    virtual void BuildAppearance() override;
};

// DIRECTOR del Builder: decide el orden de los pasos y que constructor usar
UCLASS()
class NEXUS_API UNexusEnemyAssembler : public UObject
{
    GENERATED_BODY()

public:
    void AssembleVariant(ANexusZombie* Target, ENexusZombieVariant Variant);
    void AssembleBoss(ANexusZombie* Target);

private:
    void Construct(UNexusEnemyBuilder* Builder, ANexusZombie* Target);
};