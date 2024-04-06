#include "stdafx.h"
#include "..\Sources\CSBackup.h"
#include "..\Sources\Shared\Debug_interface.h"


struct DebugOutImp : public IDebugOutput
{

	void PrintW( __int32 messageType, LPCWSTR szMessage )
	{
		std::wcout<<szMessage<<std::endl;
	}
	void PrintA( __int32 messageType, LPCSTR szMessage )
	{
		std::cout<<szMessage<<std::endl;
	}


};

typedef HRESULT ( *MYPROC )(ICSBackUp**, void* );

void _tmain()
{
	HMODULE hModule = LoadLibrary( L"D:\\Projects\\CSBackup\\Debug\\CSBackup.dll" );
	MYPROC proc;
	DebugOutImp dbg;
	
	proc = (MYPROC)GetProcAddress( hModule, "CreateCSBackUp" );

	ICSBackUp* ptr;
	HRESULT hr = proc( &ptr, (void*)(&dbg) );

	Sleep(500000);
	ptr->Release();
    
	__asm nop
	FreeLibrary( hModule );
}


