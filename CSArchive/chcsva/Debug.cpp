// Debug.cpp
//

#include "chcs.h"

#include "resource.h"
#include "DlgConfigure.h"
#include "DlgShowStream.h"

#include <conio.h>

// See the Microsoft Knowledge Base for more details.
// http://support.microsoft.com/default.aspx?scid=KB;EN-US;q164787&
//
//	hwnd - window handle that should be used as the owner window for
//	any windows your DLL creates
//	hinst - your DLL's instance handle
//	lpszCmdLine - ASCIIZ command line your DLL should parse
//	nCmdShow - describes how your DLL's windows should be displayed

void __stdcall Debug(HWND hwnd, HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow)
{
	CHCS::INITCHCS initCHCS = { sizeof(CHCS::INITCHCS) }; 
	initCHCS.procOutput = 0;
	initCHCS.dwMode = CHCS_MODE_GUI | CHCS_MODE_DATABASE;

	{
		LPSTR ptr;
		char ucCmdLine[MAX_PATH];

		// Command line init.
		strcpy(ucCmdLine, lpCmdLine);
		strupr(ucCmdLine);

		if( (ptr = strstr(ucCmdLine, "/MODE")) || 
			(ptr = strstr(ucCmdLine, "-MODE")) )
		{
			ptr = lpCmdLine + (ptr - ucCmdLine);

			switch(*(strstr(ptr, "=")+1))
			{
			default:
			case '0':
				initCHCS.dwMode |= CHCS_MODE_CLIENT;
				break;
			case '1':
				initCHCS.dwMode |= CHCS_MODE_SERVER;
				break;
			case '2':
				initCHCS.dwMode |= (CHCS_MODE_DEVICE | CHCS_MODE_DEVICENEW);
			}
		}
		else
			initCHCS.dwMode |= CHCS_MODE_SERVER;
	}

	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	::AllocConsole();
	::freopen("conout$", "w", stdout);
	::freopen("conout$", "w", stderr);
	::fprintf(stdout, "stdout: ok\n");
	::fprintf(stderr, "stderr: ok\n");
#endif
	//////////////////////////////////////////////////////////////////////////

	HRESULT hr;
	hr = CoInitialize(NULL);
	if(FAILED(hr))
	{
		MessageBox(NULL, TEXT("CoInitialize failed"),
			TEXT("Configure"), MB_OK | MB_ICONSTOP);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	CDlgConfigure configure;
	if(!configure.LoadSettings())
		configure.DoModal();

	//////////////////////////////////////////////////////////////////////////

	if(!CHCS::InitStreamManager(&initCHCS))
		return;

#ifdef _DEBUG
	Elvees::Output(Elvees::TInfo, TEXT("Press any key ..."));
	_getch();
#else
	MessageBox(NULL, TEXT(""), TEXT("Click to close"), MB_OK);
#endif

	GetManager()->ShutDown();
	GetManager()->Wait();
    
	CHCS::FreeStreamManager();
}
