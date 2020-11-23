// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include <iostream>
#include "Windows/WindowsPlatformOutputDevices.h"
#include "Misc/CommandLine.h"
#include "Windows/WindowsHWrapper.h"
#include <Misc/OutputDeviceConsole.h>
#include <Windows/WindowsPlatformApplicationMisc.h>
using namespace std;


#if IS_PROGRAM
	#if IS_MONOLITHIC
		TCHAR GInternalProjectName[64] = TEXT("LogMinimal");
		const TCHAR* GForeignEngineDir = TEXT(UE_ENGINE_DIRECTORY);
	#endif	
#endif

#if WITH_APPLICATION_CORE
static TUniquePtr<FOutputDeviceConsole>	GScopedLogConsole;
#endif



int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char*, _In_ int32 nCmdShow)
//int main(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char*, _In_ int32 nCmdShow)
//int main()
{
	FCommandLine::Set(::GetCommandLineW());

	GScopedLogConsole = TUniquePtr<FOutputDeviceConsole>(FPlatformApplicationMisc::CreateConsoleOutputDevice());

	GLogConsole = GScopedLogConsole.Get();

	GLogConsole->Show(true);

	// Init logging to disk
	FPlatformOutputDevices::SetupOutputDevices();

	UE_LOG(LogTemp, Warning, TEXT("SECURE CRTffdfdfdfdfdfdd:"));

	system("pause");
}
