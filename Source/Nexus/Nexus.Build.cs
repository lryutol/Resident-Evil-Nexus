// Source/Nexus/Nexus.Build.cs

using UnrealBuildTool;

public class Nexus : ModuleRules
{
    public Nexus(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "UMG",
            "AIModule",        // <-- NUEVO: Para usar AAIController
            "NavigationSystem", // <-- NUEVO: Para el pathfinding (MoveToActor)
            "GameplayTasks"     // <-- NUEVO: Dependencia de AIModule
        });
    }
}