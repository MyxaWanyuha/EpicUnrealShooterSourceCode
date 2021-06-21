// Copyright Saber Studio. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EpicUnrealShooterTarget : TargetRules
{
	public EpicUnrealShooterTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "EpicUnrealShooter" } );
	}
}
