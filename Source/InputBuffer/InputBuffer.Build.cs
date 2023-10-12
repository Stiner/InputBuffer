// Copyright 2017 Isaac Hsu.

using UnrealBuildTool;

public class InputBuffer : ModuleRules
{
    public InputBuffer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // ... add public include paths required here ...
        PublicIncludePaths.AddRange(new string[] {
        });

        // ... add other private include paths required here ...
        PrivateIncludePaths.AddRange(new string[] {
        });

        // ... add other public dependencies that you statically link with here ...
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "InputCore",
        });

        // ... add private dependencies that you statically link with here ...	
        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject",
            "Engine",
            "SlateCore",
        });

        // ... add any modules that your module loads dynamically here ...
        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}
