// CSLearnConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CSLearnConsole.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "HelpFunctions.h"

// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		try
		{
			std::wcout << L"This is learning console" << std::endl;
			
			CFileFind finder;
			BOOL bWorking = finder.FindFile(_T("*.*"));
			while ( bWorking ) 
			{
				bWorking = finder.FindNextFile();
				if ( finder.IsDirectory() ) 
				{
					CString s = finder.GetFileName();
			
					std::wstring dir = (LPCTSTR)(s);
					if ( IsCamDirectory( dir ) )
						ProcessCamDirecory( dir );
				}
			}		
		}
		catch( std::runtime_error & err )
		{
			CString msg( err.what() );
			std::wcout << L"  Catch:" << std::endl << (LPCWSTR)msg;
		}				
		
	}

	return nRetCode;
}



