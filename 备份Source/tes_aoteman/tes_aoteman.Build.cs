// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class tes_aoteman : ModuleRules
{
	public tes_aoteman(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"UnrealEd","AssetTools","ImageWrapper","ImageCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "EditorScriptingUtilities" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
