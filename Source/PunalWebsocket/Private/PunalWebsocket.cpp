// Copyright Epic Games, Inc. All Rights Reserved.

#include "PunalWebsocket.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
//#include <PunalWebsocketLibrary/AllHeaders.h>
//#include "PunalWebsocketLibrary/ExampleLibrary.cpp"

#define LOCTEXT_NAMESPACE "FPunalWebsocketModule"

void FPunalWebsocketModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("PunalWebsocket")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/PunalWebsocketLibrary/Win64/ExampleLibrary.dll"));
#elif PLATFORM_MAC
    //LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/PunalWebsocketLibrary/Mac/Release/libExampleLibrary.dylib"));
#elif PLATFORM_LINUX
	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/PunalWebsocketLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS

	///ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	//if (ExampleLibraryHandle)
	//{
	//	// Call the test function in the third party library that opens a message box
	//	ExampleLibraryFunction();
	//}
	//else
	//{
	//	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	//}
}

void FPunalWebsocketModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	//FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	//ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPunalWebsocketModule, PunalWebsocket)
