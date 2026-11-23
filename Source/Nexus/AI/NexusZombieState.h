#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NexusZombieState.generated.h"

class ANexusZombieAIController;

// STATE (GoF): cada comportamiento de la IA es un objeto con Entrar / Actualizar / Salir
UCLASS(Abstract)
class NEXUS_API UNexusZombieState : public UObject
{
    GENERATED_BODY()

public:
    virtual void OnEnter(ANexusZombieAIController* Brain) {}
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) {}
    virtual void OnExit(ANexusZombieAIController* Brain) {}
};

UCLASS()
class NEXUS_API UNexusWanderState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) override;
};

UCLASS()
class NEXUS_API UNexusHuntState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) override;
};

UCLASS()
class NEXUS_API UNexusChaseState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) override;
};

UCLASS()
class NEXUS_API UNexusReachState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) override;
};

UCLASS()
class NEXUS_API UNexusAttackState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
    virtual void OnTick(ANexusZombieAIController* Brain, float DeltaTime) override;
};

// Inconsciente (Tyrant derribado): no hace nada
UCLASS()
class NEXUS_API UNexusDownedState : public UNexusZombieState
{
    GENERATED_BODY()
public:
    virtual void OnEnter(ANexusZombieAIController* Brain) override;
};