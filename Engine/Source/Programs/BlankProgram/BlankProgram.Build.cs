using UnrealBuildTool;
public class BlankProgram : ModuleRules
{
	public BlankProgram(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseUnity = false;

        PrivateDependencyModuleNames.Add("Core");
    }
}