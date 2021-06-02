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


#include <Serialization/MemoryWriter.h>
#include <Serialization/MemoryReader.h>

struct FArchive1
{
	//FORCENOINLINE /*friend*/ FArchive1& operator<<(FArchive1& Ar, int32& Value);
private:
	int foo;
};

FArchive1& operator<<(FArchive1& Ar, int32& Value)
{
	//cout << Ar.foo << endl;
	return Ar;
}
struct FV
{
	float X; float Y; float Z;

};

struct FTestSerializeStruct
{
	int32 IData;
	float FData;
	bool BData;
	FVector Vec;

	// 序列化/反序列化的数据传输的定义
	void Serialize(FArchive& Ar)
	{
		Ar << IData;
		Ar << FData;
		Ar << BData;
		Ar << Vec;
	}
	FORCENOINLINE friend FArchive& operator<<(FArchive& Ar, FTestSerializeStruct& Data)
	{
		Data.Serialize(Ar);
		return Ar;
	}
};

int main()
{
	FTestSerializeStruct Obj{ 10, 50.f, true, {1.f,2.f,3.f} };

	// 序列化
	TArray<uint8> Buffer;
	FMemoryWriter MW(Buffer);
	MW << Obj;
	//Obj.Serialize(MW);

	// 反序列化
	FMemoryReader MR(Buffer);
	FTestSerializeStruct NewObj;
	MR << NewObj;
	//NewObj.Serialize(MR);

	//check(NewObj == Obj);



	int32 bar = 1;
	FArchive1 fa1;
	fa1 << bar;

	system("pause");
}

