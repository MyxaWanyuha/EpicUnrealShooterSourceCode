// Copyright Saber Studio. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EpicUnrealShooterEditorTarget : TargetRules
{
	public EpicUnrealShooterEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "EpicUnrealShooter" } );
	}
}
