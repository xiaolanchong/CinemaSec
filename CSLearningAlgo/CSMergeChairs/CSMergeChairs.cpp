// CSMergeChairs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CSMergeChairs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "ChairImageContainer.h"

// The one and only application object

CWinApp theApp;

int ProcessOneChair( int camNo, int chairNo )
{
	int retValue = -1;
	
	std::wstring resultName;
	
	bool first = true;

    CFileFind finder;

	std::auto_ptr < avideolib::IVideoWriter > pWriter;
	
	std::auto_ptr < avideolib::IVideoReader > pReader;

	pWriter.reset( avideolib::CreateAVideoWriter( L"tmp.avd", 0, false, INVERT_AXIS_Y ) );

	
	// find all "clusterized files"
	CSFileNameParser parser;
	
	std::wstring ptrn = parser.GetCamStr( camNo ) + L"\\" +  parser.GetClusterizedFileNamePattern( camNo, chairNo );

	BOOL bWorking = finder.FindFile( ptrn.c_str() );
	while ( bWorking ) 
	{
		bWorking = finder.FindNextFile();
		if ( !finder.IsDirectory() ) 
		{
			std::wstring name = parser.GetCamStr( camNo ) + L"\\" + (LPCTSTR)finder.GetFileName();

			/**/
			if ( first )
			{
				resultName = parser.GetCamStr( camNo ) + L"\\" + parser.GetMergedFromClusterizedName( name );
				first = false;
			}

			/**/
			pReader.reset( avideolib::CreateAVideoReader( name.c_str(), 0, INVERT_AXIS_Y ) );

			avideolib::IVideoReader::ReturnCode code;
			Arr2ub frame;
			do 
			{
				code = pReader->ReadFrame( &frame, -1, 0, 0 );
				if ( code == avideolib::IVideoReader::GENERAL_ERROR )
				{
					throw std::runtime_error (" reading error ");
				}
				
				pWriter->WriteFrame( &frame, 0 );
			}
			while ( code == avideolib::IVideoReader::NORMAL );
			pReader.reset( );
			_wremove( name.c_str() );
		}		
	}

	finder.Close();

	// find all "merged files"
    ptrn = parser.GetCamStr( camNo ) + L"\\" +  parser.GetMergedFileNamePattern( camNo, chairNo );

	bWorking = finder.FindFile( ptrn.c_str() );
	while ( bWorking ) 
	{
		bWorking = finder.FindNextFile();
		if ( !finder.IsDirectory() ) 
		{
			std::wstring name = parser.GetCamStr( camNo ) + L"\\" + (LPCTSTR)finder.GetFileName();
			pReader.reset( avideolib::CreateAVideoReader( name.c_str(), 0, INVERT_AXIS_Y ) );

			avideolib::IVideoReader::ReturnCode code;
			Arr2ub frame;
	
			do 
			{
				code = pReader->ReadFrame( &frame, -1, 0, 0 );
				if ( code == avideolib::IVideoReader::GENERAL_ERROR )
				{
					throw std::runtime_error (" reading error ");
				}

				pWriter->WriteFrame( &frame, 0 );
			}
			while ( code == avideolib::IVideoReader::NORMAL );
			pReader.reset( );
			_wremove( name.c_str() );

		}
	}
	finder.Close();


	pWriter.reset( );
	_wrename( L"tmp.avd", resultName.c_str() );
	_wremove( L"tmp.avd" );

	return retValue;
}

int ProcessOneCamDir( int camNo )
{
	int retValue = -1;

	CSFileNameParser parser;
	std::wstring ptrn = parser.GetCamStr( camNo ) + L"\\" + parser.GetClusterizedFileNamePattern( camNo );

	CFileFind finder;
	BOOL bWorking = finder.FindFile( ptrn.c_str() );

	while ( bWorking ) 
	{
		bWorking = finder.FindNextFile();
		if ( !finder.IsDirectory() ) 
		{
			std::wstring file = (LPCTSTR)finder.GetFileName();
			int chairNo = parser.GetChairNNN( file );

			if ( chairNo >= 0 && chairNo < 1000 )
				retValue = ProcessOneChair( camNo, chairNo );
		}
	}

	
	ptrn = parser.GetCamStr( camNo ) + L"\\" + parser.GetRefinedFileNamePattern( camNo );
	finder;
	bWorking = finder.FindFile( ptrn.c_str() );



	ChairImageContainer container;

	std::wstring filename = parser.GetCamStr( camNo ) + L"_chair_templates.xml";
	try
	{
		container.Load( filename );
	}
	catch (...) 
	{
	}

	bool bNeedToSave = false;

	std::auto_ptr < avideolib::IVideoReader > pReader;
	while ( bWorking ) 
	{
		bWorking = finder.FindNextFile();
		if ( !finder.IsDirectory() ) 
		{
			std::wstring file = parser.GetCamStr( camNo ) + L"\\" + (LPCTSTR)finder.GetFileName();

			int chairNo = parser.GetChairNNN( file );
			
			if ( chairNo >= 0 && chairNo < 1000 )
			{
				bNeedToSave = true;

				pReader.reset( avideolib::CreateAVideoReader( file.c_str(), 0, INVERT_AXIS_Y ) );
				avideolib::IVideoReader::ReturnCode code;
				Arr2ub frame;
				do 
				{
					code = pReader->ReadFrame( &frame, -1, 0, 0 );
					if ( code == avideolib::IVideoReader::GENERAL_ERROR )
					{
						throw std::runtime_error (" reading error ");
					}
          std::wstring chairText( L"chair" );
					container.AddChairImage( camNo, chairNo, chairText, frame );
				}
				while ( code == avideolib::IVideoReader::NORMAL );
			}
		}
	}

	if ( bNeedToSave )
	{
		container.Save( filename );
	}

	return retValue;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
  argc;argv;envp;
	std::wcout.imbue( std::locale ( ".866" ) );

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
			std::wcout << L"Merging chairs" << std::endl;

			CSFileNameParser parser;

			CFileFind finder;
			BOOL bWorking = finder.FindFile(_T("*.*"));
			while ( bWorking ) 
			{
				bWorking = finder.FindNextFile();
				if ( finder.IsDirectory() ) 
				{
					std::wstring dir = (LPCTSTR)finder.GetFileName();
					int camNo = parser.IsCamNN( dir );
					if ( camNo > 0 )
						ProcessOneCamDir( camNo );
				}
			}		
		}
		catch( std::runtime_error & err )
		{
			CString msg( err.what() );
			std::wcout << L"  Catch:" << std::endl << (LPCWSTR)msg;
		}			
	}
/*
	ChairImageContainer container;
	container.Load( std::wstring( L"cam18.xml") );

	std::vector < Arr2ub > images;
	container.GetChairImages( 18, 105, images );

	for (int i = 0; i < (int) images.size(); i++ )
	{
		wchar_t buf[100];
		std::wstring s = std::wstring( _itow(i, buf, 10 )) + std::wstring( L".bmp" );
		csutility::SaveByteImageEx( s.c_str(), &images[i], INVERT_AXIS_Y, false );
	}
*/
}
