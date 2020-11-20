using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Desktop)]
public class BlankProgramTarget : TargetRules
{
	public BlankProgramTarget(TargetInfo Target) : base(Target)
	{
        Type = TargetType.Program;
        LinkType = TargetLinkType.Modular;
        LaunchModuleName = "BlankProgram";

        bCompileICU = false;
        bCompileAgainstEngine = false;

        // UnrealHeaderTool is a console application, not a Windows app (sets entry point to main(), instead of WinMain())
        //bIsBuildingConsoleApplication = true;
    }
}