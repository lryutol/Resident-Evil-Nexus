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
            "AIModule",
            "NavigationSystem",
            "GameplayTasks"
        });
    }
}