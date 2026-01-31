// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheOtleyRun : ModuleRules
{
	public TheOtleyRun(ReadOnlyTargetRules Target) : base(Target)
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
			"TheOtleyRun",
			"TheOtleyRun/Variant_Platforming",
			"TheOtleyRun/Variant_Platforming/Animation",
			"TheOtleyRun/Variant_Combat",
			"TheOtleyRun/Variant_Combat/AI",
			"TheOtleyRun/Variant_Combat/Animation",
			"TheOtleyRun/Variant_Combat/Gameplay",
			"TheOtleyRun/Variant_Combat/Interfaces",
			"TheOtleyRun/Variant_Combat/UI",
			"TheOtleyRun/Variant_SideScrolling",
			"TheOtleyRun/Variant_SideScrolling/AI",
			"TheOtleyRun/Variant_SideScrolling/Gameplay",
			"TheOtleyRun/Variant_SideScrolling/Interfaces",
			"TheOtleyRun/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
