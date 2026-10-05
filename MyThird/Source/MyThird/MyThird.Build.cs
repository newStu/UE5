// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyThird : ModuleRules
{
	public MyThird(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
