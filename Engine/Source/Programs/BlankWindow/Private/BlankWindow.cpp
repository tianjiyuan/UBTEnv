
#include "CoreMinimal.h"
#include <iostream>
#include "Windows/WindowsPlatformOutputDevices.h"
#include "Misc/CommandLine.h"
#include "Windows/WindowsHWrapper.h"
#include <Misc/OutputDeviceConsole.h>
#include <Windows/WindowsPlatformApplicationMisc.h>
#include <Misc/OutputDeviceRedirector.h>
#include <Windows/WindowsWindow.h>
#include <Misc/ConfigCacheIni.h>
using namespace std;


#if IS_PROGRAM
	#if IS_MONOLITHIC
		TCHAR GInternalProjectName[64] = TEXT("BlankWindow");
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
	// 搭建Log环境
	FCommandLine::Set(::GetCommandLineW());
	GScopedLogConsole = TUniquePtr<FOutputDeviceConsole>(FPlatformApplicationMisc::CreateConsoleOutputDevice());
	GLogConsole = GScopedLogConsole.Get();
	GLogConsole->Show(true);
	FPlatformOutputDevices::SetupOutputDevices();
	UE_LOG(LogTemp, Warning, TEXT("Hello World!"));


	FConfigCacheIni::InitializeConfigSystem();

	hInstance = hInInstance;

	FPlatformApplicationMisc::SetHighDPIMode();
	TSharedPtr<class GenericApplication> PlatformApplication = MakeShareable(FPlatformApplicationMisc::CreateApplication());

	TSharedRef< FGenericWindowDefinition > Definition = MakeShareable(new FGenericWindowDefinition());
	Definition->HeightDesiredOnScreen = 500.f;
	Definition->WidthDesiredOnScreen = 500.f;
	Definition->Opacity = 1.f;
	Definition->Title = FWindowsWindow::AppWindowClass;
	Definition->IsRegularWindow = true;
	Definition->AppearsInTaskbar = true;
	Definition->AcceptsInput = true;
	Definition->ActivationPolicy = EWindowActivationPolicy::Always;
	Definition->FocusWhenFirstShown = true;
	Definition->HasCloseButton = true;
	Definition->SupportsMaximize = true;
	Definition->SupportsMinimize = true;
	Definition->ExpectedMaxHeight = -1;
	Definition->ExpectedMaxWidth = -1;
	Definition->CornerRadius = 6;

	TSharedRef< FGenericWindow > NewWindow = PlatformApplication->MakeWindow();

	FVector2D WndSize(Definition->WidthDesiredOnScreen, Definition->HeightDesiredOnScreen);
	NewWindow->AdjustCachedSize(WndSize);

	PlatformApplication->InitializeWindow(NewWindow, Definition, /*NativeParent*/nullptr, /*bShowImmediately*/true);

	NewWindow->BringToFront(true);

	NewWindow->SetText(FWindowsWindow::AppWindowClass);

	//NewWindow->Maximize();

	NewWindow->Show();


	while (!IsEngineExitRequested())
	{
		FPlatformApplicationMisc::PumpMessages(true);
	}

	//MSG msg = { };
	//while (GetMessageW(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	system("pause");
}
