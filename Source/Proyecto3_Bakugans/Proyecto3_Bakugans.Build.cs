// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Proyecto3_Bakugans : ModuleRules
{
	public Proyecto3_Bakugans(ReadOnlyTargetRules Target) : base(Target)
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
			"Proyecto3_Bakugans",
			"Proyecto3_Bakugans/Variant_Platforming",
			"Proyecto3_Bakugans/Variant_Platforming/Animation",
			"Proyecto3_Bakugans/Variant_Combat",
			"Proyecto3_Bakugans/Variant_Combat/AI",
			"Proyecto3_Bakugans/Variant_Combat/Animation",
			"Proyecto3_Bakugans/Variant_Combat/Gameplay",
			"Proyecto3_Bakugans/Variant_Combat/Interfaces",
			"Proyecto3_Bakugans/Variant_Combat/UI",
			"Proyecto3_Bakugans/Variant_SideScrolling",
			"Proyecto3_Bakugans/Variant_SideScrolling/AI",
			"Proyecto3_Bakugans/Variant_SideScrolling/Gameplay",
			"Proyecto3_Bakugans/Variant_SideScrolling/Interfaces",
			"Proyecto3_Bakugans/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
