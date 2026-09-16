// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PropHuntJOchoa : ModuleRules
{
	public PropHuntJOchoa(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"PropHuntJOchoa",
			"PropHuntJOchoa/Variant_Platforming",
			"PropHuntJOchoa/Variant_Platforming/Animation",
			"PropHuntJOchoa/Variant_Combat",
			"PropHuntJOchoa/Variant_Combat/AI",
			"PropHuntJOchoa/Variant_Combat/Animation",
			"PropHuntJOchoa/Variant_Combat/Gameplay",
			"PropHuntJOchoa/Variant_Combat/Interfaces",
			"PropHuntJOchoa/Variant_Combat/UI",
			"PropHuntJOchoa/Variant_SideScrolling",
			"PropHuntJOchoa/Variant_SideScrolling/AI",
			"PropHuntJOchoa/Variant_SideScrolling/Gameplay",
			"PropHuntJOchoa/Variant_SideScrolling/Interfaces",
			"PropHuntJOchoa/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
