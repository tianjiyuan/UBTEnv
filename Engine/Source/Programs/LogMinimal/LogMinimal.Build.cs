using UnrealBuildTool;
public class LogMinimal : ModuleRules
{
	public LogMinimal(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseUnity = false;

        PrivateDependencyModuleNames.Add("Core");
        PrivateDependencyModuleNames.Add("ApplicationCore");
    }
}