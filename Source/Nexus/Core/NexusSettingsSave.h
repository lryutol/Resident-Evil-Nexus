#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputCoreTypes.h"
#include "NexusSettingsSave.generated.h"

// Una tecla personalizada guardada por el jugador
USTRUCT(BlueprintType)
struct FNexusKeyBinding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FName Name = NAME_None;
	UPROPERTY(BlueprintReadWrite) bool bIsAxis = false;
	UPROPERTY(BlueprintReadWrite) float Scale = 1.0f;
	UPROPERTY(BlueprintReadWrite) FKey Key;
};

// Datos de configuracion que se guardan en disco
UCLASS(BlueprintType)
class NEXUS_API UNexusSettingsSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Audio") float MasterVolume = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Audio") float MusicVolume = 0.7f;
	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Audio") float SfxVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Controls") float MouseSensitivity = 1.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Interface") float HudScale = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Nexus|Interface") bool bShowCrosshair = true;

	UPROPERTY() TArray<FNexusKeyBinding> KeyBindings;
};