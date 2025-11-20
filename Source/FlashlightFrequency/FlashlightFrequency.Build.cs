// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FlashlightFrequency : ModuleRules
{
	public FlashlightFrequency(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate", 
			"GameplayTasks", 
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.Add("FlashlightFrequency");
		
		PrivateDependencyModuleNames.Add("OnlineSubsystem");
		
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
