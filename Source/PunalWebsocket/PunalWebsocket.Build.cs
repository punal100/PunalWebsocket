// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PunalWebsocket : ModuleRules
{
	public PunalWebsocket(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		Add_Include_And_Dependency(Target);

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Boost",
                "OpenSSL",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}

	public void Add_Include_And_Dependency(ReadOnlyTargetRules Target)
	{
		//string Boost_Beast_Includes_Path = Path.Combine(ModuleDirectory, "../", "ThirdParty", "PunalWebsocketLibrary");
        //Boost_Beast_Includes_Path = Path.Combine(ModuleDirectory);
		//
        //PublicIncludePaths.AddRange(
        //    new string[] {
		//		// ... add public include paths required here ...
		//		Boost_Beast_Includes_Path
        //    }
        //    );
    }
}
