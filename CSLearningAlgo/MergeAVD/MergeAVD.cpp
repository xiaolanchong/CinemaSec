// MergeAVD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MergeAVD.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

//using namespace std;

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
		if ( argc != 3 )
		{
			std::wcout << L" Usage: MergeAVD.exe input_directory output_filename" << std::endl;
			std::wcout << L" file output_filename will be created in input_directory" << std::endl;
			return 1;
		}

		std::wstring outDir = argv[1];

		if ( outDir[outDir.length() - 1 ]  == (std::wstring(L"\\"))[0])
		{
			outDir.resize( outDir.length() - 1 );
		}
		std::wstring outputFilename = outDir + L"\\" + argv[2];

		CFileFind finder;

		std::wstring findPattern = outDir + L"\\*.avd";
		BOOL bWorking = finder.FindFile( findPattern.c_str() );

		std::auto_ptr < avideolib::IVideoWriter > pWriter = std::auto_ptr < avideolib::IVideoWriter > ( avideolib::CreateAVideoWriter( outputFilename.c_str(), 0, false, true));
		while (bWorking)
		{
			bWorking = finder.FindNextFile();			
			std::wstring str = outDir + L"\\" + std::wstring( finder.GetFileName() );
			
			if ( str == outputFilename )
			{
				std::wcout << std::endl << L"Warning! the same input and output filenames" << std::endl;
				std::wcout << L"  " << str.c_str() << std::endl << std::endl;
				continue;
			}
			std::wcout << L" Processing: "<< str.c_str() << L" " << std::endl;
			
			std::auto_ptr < avideolib::IVideoReader > pReader = std::auto_ptr < avideolib::IVideoReader > ( avideolib::CreateAVideoReader( str.c_str(), 0, true ) );
			// now write all frames from input to aoutput
			{
				Arr2ub tmp;
				while( pReader->ReadFrame( &tmp, -1, 0, 0 ) != avideolib::IVideoReader::END_IS_REACHED )
				{
					pWriter->WriteFrame( &tmp, 0 );
				}
			}
		}
	}

	return nRetCode;
}
