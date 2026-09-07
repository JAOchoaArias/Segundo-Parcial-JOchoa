// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PropHuntExpress : ModuleRules
{
	public PropHuntExpress(ReadOnlyTargetRules Target) : base(Target)
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
			"PropHuntExpress",
			"PropHuntExpress/Variant_Platforming",
			"PropHuntExpress/Variant_Platforming/Animation",
			"PropHuntExpress/Variant_Combat",
			"PropHuntExpress/Variant_Combat/AI",
			"PropHuntExpress/Variant_Combat/Animation",
			"PropHuntExpress/Variant_Combat/Gameplay",
			"PropHuntExpress/Variant_Combat/Interfaces",
			"PropHuntExpress/Variant_Combat/UI",
			"PropHuntExpress/Variant_SideScrolling",
			"PropHuntExpress/Variant_SideScrolling/AI",
			"PropHuntExpress/Variant_SideScrolling/Gameplay",
			"PropHuntExpress/Variant_SideScrolling/Interfaces",
			"PropHuntExpress/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
