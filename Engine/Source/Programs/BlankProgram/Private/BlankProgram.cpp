// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/App.h"
#include "Misc/OutputDeviceError.h"
#include "HAL/ExceptionHandling.h"
#include "HAL/PlatformMallocCrash.h"
#include "Windows/WindowsHWrapper.h"
#include <shellapi.h>

#include <iostream>
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "Misc/OutputDeviceConsole.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include <Misc/CoreDelegates.h>
#include <Misc/ConfigCacheIni.h>
#include "Windows/WindowsPlatformSplash.h"
#include "Windows/WindowsWindow.h"
using namespace std;

#if IS_PROGRAM
	#if IS_MONOLITHIC
		TCHAR GInternalProjectName[64] = TEXT("BlankProgram");
		const TCHAR* GForeignEngineDir = TEXT(UE_ENGINE_DIRECTORY);
	#endif	
#endif

#if WITH_APPLICATION_CORE
static TUniquePtr<FOutputDeviceConsole>	GScopedLogConsole;
#endif


//int main()
//{
//	cout << "Hello World" << endl;
//
//	TArray<FVector> tv;
//	tv.Add({1,2,3});
//	tv.Add({2,3,4});
//
//	wcout<< *tv.Top().ToString() << endl;
//
////#if WITH_APPLICATION_CORE
////	{
////		SCOPED_BOOT_TIMING("CreateConsoleOutputDevice");
////		// Initialize log console here to avoid statics initialization issues when launched from the command line.
////		GScopedLogConsole = TUniquePtr<FOutputDeviceConsole>(FPlatformApplicationMisc::CreateConsoleOutputDevice());
////		GError = FPlatformApplicationMisc::GetErrorOutputDevice();
////		GWarn = FPlatformApplicationMisc::GetFeedbackContext();
////
////		FPlatformApplicationMisc::LoadPreInitModules();
////	}
////#endif
//
//
//	system("pause");
//}





/**
 * The command-line invocation string, processed using the standard Windows CommandLineToArgvW implementation.
 * This need to be a static global to avoid object unwinding errors in WinMain when SEH is enabled.
 */
static FString GSavedCommandLine;
TSharedPtr<class GenericApplication> PlatformApplication;

/**
 * Handler for CRT parameter validation. Triggers error
 *
 * @param Expression - the expression that failed crt validation
 * @param Function - function which failed crt validation
 * @param File - file where failure occured
 * @param Line - line number of failure
 * @param Reserved - not used
 */
void InvalidParameterHandler(const TCHAR* Expression,
	const TCHAR* Function,
	const TCHAR* File,
	uint32 Line,
	uintptr_t Reserved)
{
	UE_LOG(LogTemp, Fatal, TEXT("SECURE CRT: Invalid parameter detected.\nExpression: %s Function: %s. File: %s Line: %d\n"),
		Expression ? Expression : TEXT("Unknown"),
		Function ? Function : TEXT("Unknown"),
		File ? File : TEXT("Unknown"),
		Line);
}

/**
 * Setup the common debug settings
 */
void SetupWindowsEnvironment(void)
{
	// all crt validation should trigger the callback
	_set_invalid_parameter_handler(InvalidParameterHandler);

#if UE_BUILD_DEBUG
	// Disable the message box for assertions and just write to debugout instead
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	// don't fill buffers with 0xfd as we make assumptions for FNames st we only use a fraction of the entire buffer
	_CrtSetDebugFillThreshold(0);
#endif
}

bool ProcessCommandLine()
{
	int argc = 0;
	LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
	if (argv != nullptr)
	{
		// Reconstruct our command line string in a format suitable for consumption by the FParse class
		GSavedCommandLine = "";
		for (int32 Option = 1; Option < argc; Option++)
		{
			GSavedCommandLine += TEXT(" ");

			// Inject quotes in the correct position to preserve arguments containing whitespace
			FString Temp = argv[Option];
			if (Temp.Contains(TEXT(" ")))
			{
				int32 Quote = 0;
				if (Temp.StartsWith(TEXT("-")))
				{
					int32 Separator;
					if (Temp.FindChar('=', Separator))
					{
						Quote = Separator + 1;
					}
				}
				Temp = Temp.Left(Quote) + TEXT("\"") + Temp.Mid(Quote) + TEXT("\"");
			}
			GSavedCommandLine += Temp;
		}

		// Free memory allocated for CommandLineToArgvW() arguments
		::LocalFree(argv);
		return true;
	}

	return false;
}


int32 GuardedMain(const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, int32 nCmdShow)
{
	BootTimingPoint("DefaultMain");

	// Super early init code. DO NOT MOVE THIS ANYWHERE ELSE!
	FCoreDelegates::GetPreMainInitDelegate().Broadcast();

// Engine PreInit -------------------------------------------------------------------------------------------------------------

	TRACE_REGISTER_GAME_THREAD(FPlatformTLS::GetCurrentThreadId());
#if CPUPROFILERTRACE_ENABLED
	FCpuProfilerTrace::Init(FParse::Param(CmdLine, TEXT("cpuprofilertrace")));
#endif

	SCOPED_BOOT_TIMING("FEngineLoop::PreInit");

#if PLATFORM_WINDOWS
	// Register a handler for Ctrl-C so we've effective signal handling from the outset.
	FWindowsPlatformMisc::SetGracefulTerminationHandler();
#endif // PLATFORM_WINDOWS

	FMemory::SetupTLSCachesOnCurrentThread();

	// disable/enable LLM based on commandline
	{
		SCOPED_BOOT_TIMING("LLM Init");
		LLM(FLowLevelMemTracker::Get().ProcessCommandLine(CmdLine));
	}
	LLM_SCOPE(ELLMTag::EnginePreInitMemory);

	{
		SCOPED_BOOT_TIMING("InitTaggedStorage");
		FPlatformMisc::InitTaggedStorage(1024);
	}

	if (FParse::Param(CmdLine, TEXT("UTF8Output")))
	{
		FPlatformMisc::SetUTF8Output();
	}

	// Switch into executable's directory.
	FPlatformProcess::SetCurrentWorkingDirectoryToBaseDir();

	// this is set later with shorter command lines, but we want to make sure it is set ASAP as some subsystems will do the tests themselves...
	// also realize that command lines can be pulled from the network at a slightly later time.
	if (!FCommandLine::Set(CmdLine))
	{
		// Fail, shipping builds will crash if setting command line fails
		return -1;
	}

#if WITH_APPLICATION_CORE
	{
		SCOPED_BOOT_TIMING("CreateConsoleOutputDevice");
		// Initialize log console here to avoid statics initialization issues when launched from the command line.
		GScopedLogConsole = TUniquePtr<FOutputDeviceConsole>(FPlatformApplicationMisc::CreateConsoleOutputDevice());
		GError = FPlatformApplicationMisc::GetErrorOutputDevice();
		GWarn = FPlatformApplicationMisc::GetFeedbackContext();

	}
#endif
	FPlatformApplicationMisc::LoadPreInitModules();

	FPlatformApplicationMisc::LoadStartupModules();

	FConfigCacheIni::InitializeConfigSystem();

	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);


	FPlatformApplicationMisc::SetHighDPIMode();





	PlatformApplication = MakeShareable(FPlatformApplicationMisc::CreateApplication());

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
	//FPlatformSplash::Show();

	NewWindow->BringToFront(true);

	NewWindow->SetText(FWindowsWindow::AppWindowClass);

	//NewWindow->Maximize();

	NewWindow->Show();

	//const float DPIScale = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(DisplayMetrics.PrimaryDisplayWorkAreaRect.Left, DisplayMetrics.PrimaryDisplayWorkAreaRect.Top);

//Engine Tick -------------------------------------------------------------------------------------------------------------

	while (!IsEngineExitRequested())
	{
		//FPlatformApplicationMisc::PumpMessages(true);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char*, _In_ int32 nCmdShow)
{
	cout << "Hello World" << endl;

	TRACE_BOOKMARK(TEXT("WinMain.Enter"));

	// Setup common Windows settings
	SetupWindowsEnvironment();

	int32 ErrorLevel = 0;
	hInstance = hInInstance;
	const TCHAR* CmdLine = ::GetCommandLineW();

	// Attempt to process the command-line arguments using the standard Windows implementation
	// (This ensures behavior parity with other platforms where argc and argv are used.)
	if (ProcessCommandLine())
	{
		CmdLine = *GSavedCommandLine;
	}


	//// Register the window class.
	//const wchar_t CLASS_NAME[] = L"Sample Window Class";

	//WNDCLASS wc = { };
	//wc.lpfnWndProc = WindowProc;
	//wc.hInstance = hInInstance;
	//wc.lpszClassName = CLASS_NAME;
	//RegisterClass(&wc);

	//int a = WS_OVERLAPPEDWINDOW;
	//HWND hwnd = CreateWindowEx(
	//	/*416*/262400,                              // Optional window styles.
	//	CLASS_NAME,                     // Window class
	//	L"Learn to Program Windows",    // Window text
	//	/*WS_OVERLAPPEDWINDOW*//*2248146944*/a,            // Window style
	//	// Size and position
	//	0, 0, 500, 500,
	//	NULL,       // Parent window    
	//	NULL,       // Menu
	//	hInInstance,  // Instance handle
	//	NULL        // Additional application data
	//);

	//ShowWindow(hwnd, SW_SHOW);

	//MSG msg = { };
	//while (1) {}



	//ErrorLevel = GuardedMain(CmdLine, hInInstance, hPrevInstance, nCmdShow);

	system("pause");
}
