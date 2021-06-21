// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EpicUnrealShooterServerTarget : TargetRules //Change this line according to the name of your project
{
    public EpicUnrealShooterServerTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project
    {
        Type = TargetType.Server;
        //DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("EpicUnrealShooter"); //Change this line according to the name of your project
    }
}