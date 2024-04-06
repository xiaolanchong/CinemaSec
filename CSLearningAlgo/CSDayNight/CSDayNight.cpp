// CSDayNight.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CSDayNight.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;


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
			/// Õ≈ «¿¡”ƒ‹ Œ¡ÕŒ¬»“‹ DayNight.cpp / DayNight.h ËÁ CSAlgo!
			std::wcout << L"day-night learning console" << std::endl;
			if ( argc != 3 )
			{
				std::wcout << L"Usgae: CSDayNight.exe video.avd marking.txt" << std::endl;
				std::wcout << L"Marking.txt format is as follows:" << std::endl;
				std::wcout << L"it is a text file with 2 columns of ineteger numbers, day-begin, day-end" << std::endl;
				std::wcout << L"Example." << std::endl;
				std::wcout << L"daynight.txt:" << std::endl;
				std::wcout << L"100 240" << std::endl;
				std::wcout << L"500 670" << std::endl;
			}

			//load vector of "brakpoints" from marking.txt file. 
			
			std::ifstream markingfile;

			// here we need argv[2]
			markingfile.open( "c:\\Documents and Settings\\boltnev\\ÃÓË ‰ÓÍÛÏÂÌÚ˚\\Visual Studio Projects\\CSLearningAlgo\\CSLearnConsole\\Unicode Debug\\marking.txt" );
			
			std::istream_iterator<int> dataBegin( markingfile );
			std::istream_iterator<int> dataEnd;
			std::vector < int > breakpoints( dataBegin, dataEnd );


			//When we reach next breakpoint we change the value of boolean variable bIsNight. We think it is night by default.
			bool bIsNight = false;


			//create reader
			std::wstring videoFile = argv[1];
			std::auto_ptr < avideolib::IVideoReader > pReader = std::auto_ptr < avideolib::IVideoReader > ( avideolib::CreateAVideoReader( videoFile.c_str(), 0, true ) );

			// create algo
			DayNight algo;

			int frame = 0;
			int kBreak = 0;

			Arr2ub tmp;
			while( pReader->ReadFrame(&tmp, -1, 0, 0 ) != avideolib::IVideoReader::END_IS_REACHED )
			{
                if ( frame == breakpoints[kBreak] )
				{
					//When we reach next breakpoint we change the value of boolean variable bIsNight. We think it is night by default.
					kBreak++;
					bIsNight = !bIsNight;
				}
				algo.Learn( tmp, !bIsNight );
			}

			// Get histograms and ???????????????????


		}
		catch( std::runtime_error & err )
		{
			CString msg( err.what() );
			std::wcout << L"catch:" << std::endl << (LPCWSTR)msg;

		}		


		// TODO: code your application's behavior here.
	}

	return nRetCode;
}
