// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"

#if IS_PROGRAM
	#if IS_MONOLITHIC
		TCHAR GInternalProjectName[64] = TEXT("BlankProgram");
		const TCHAR* GForeignEngineDir = TEXT(UE_ENGINE_DIRECTORY);
	#endif	
#endif

#include <iostream>

using namespace std;

int main()
{
	cout << "Hello World" << endl;

	TArray<FVector> tv;
	tv.Add({1,2,3});
	tv.Add({2,3,4});

	wcout<< *tv.Top().ToString() << endl;

	system("pause");
}

