using UnrealBuildTool;
public class BlankWindow : ModuleRules
{
	public BlankWindow(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseUnity = false;

        PrivateDependencyModuleNames.Add("Core");
        PrivateDependencyModuleNames.Add("ApplicationCore");
    }
}