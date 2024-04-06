#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp      theApp;
std::fstream g_log;

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void PrintMessage( const WStr & s )
{
	USES_CONVERSION;
	if (g_log.is_open()) g_log << W2CA(s.c_str()) << std::endl;
	std::wcout << s << std::endl;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool DeleteVideoFile( const WStr & fname )
{
	if (_wremove( fname.c_str() ) != 0)
	{
		WStrStream s;
		s << L"Failed to delete file: " << fname << std::endl;
		PrintMessage( s.str() );
		return false;
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool GetWriterName( const WStr & videoFile, int index, WStr & writerName )
{
	wchar_t chairNNN[128];
	swprintf( chairNNN, L"chair%03d.avd", index );

	WStr::size_type namePos = videoFile.find_last_of( L"\\/" );
	WStr::size_type index1 = videoFile.find( L"cam"  , ((namePos != WStr::npos) ? namePos : 0) );
	WStr::size_type index2 = videoFile.find( L"video", ((namePos != WStr::npos) ? namePos : 0) );

	if ((index1 == WStr::npos) || (index2 == WStr::npos) || (index2 <= index1))
	{
		WStrStream s;
		s << L"Wrong name of video file:" << videoFile << std::endl;
		PrintMessage( s.str() );
		return false;
	}

	writerName = videoFile;
	writerName.erase( index2 );
	writerName += chairNNN;
	return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool GetChairPicture( const BaseChairEx & chair, const Arr2ub & frame, Arr2ub & chairPic )
{
	ARect rect = (chair.rect & frame.rect());
	int   w = rect.width();
	int   h = rect.height();
	int   x0 = rect.x1;
	int   y0 = rect.y1;

	if (rect != chair.rect)
	{
		WStrStream s;
		s << L"Badness: chair rectangle gets out of frame rectangle" << std::endl;
		PrintMessage( s.str() );
	}

	chairPic.resize( w, h );
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			Vec2i pt( x+x0, y+y0 );
			chairPic( x, y ) = (chair.region.inside( pt )) ? frame[pt] : (ubyte)0;
		}
	}
	return (rect == chair.rect);
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool SplitOneVideoFile( const WStr & videoFile, const BaseChairExArr & chairs )
{
	WStr                                   writerName;
	std::auto_ptr<avideolib::IVideoReader> pReader;
	std::vector<avideolib::IVideoWriter*>  writers;
	Arr2ub                                 frame, chairImg;
	avideolib::IVideoReader::ReturnCode    res;
	bool                                   ok = true;
	std::list<WStr>                        fileLst;

	try
	{
		// Open source video file.
		pReader.reset( avideolib::CreateAVideoReader( videoFile.c_str(), 0, INVERT_AXIS_Y ) );
		if (pReader.get() == 0)
		{
			WStrStream s;
			s << L"Failed to open video file for reading: " << std::endl << videoFile << std::endl;
			PrintMessage( s.str() );
			throw std::runtime_error("");
		}

		// Open destination video files.
		writers.resize( chairs.size(), (avideolib::IVideoWriter*)0 );
		{
			for (int i = 0; i < (int)(writers.size()); i++)
			{
				GetWriterName( videoFile, (chairs[i]).index, writerName );
				writers[i] = avideolib::CreateAVideoWriter( writerName.c_str(), 0, false, INVERT_AXIS_Y );
				if (writers[i] == 0)
				{
					WStrStream s;
					s << L"Failed to open video file for writing: " << std::endl << writerName << std::endl;
					PrintMessage( s.str() );
					throw std::runtime_error("");
				}
				else fileLst.push_back( writerName );
			}
		}

		// Extract chair images from the source sequence and store them into destination ones.
		do
		{
			res = pReader->ReadFrame( &frame, -1, 0, 0 );
			if ((res != avideolib::IVideoReader::NORMAL) && (res != avideolib::IVideoReader::END_IS_REACHED))
			{
				WStrStream s;
				s << L"Corrupted video file: " << std::endl << videoFile << std::endl;
				PrintMessage( s.str() );
				throw std::runtime_error("");
			}

			for (int i = 0; i < (int)(chairs.size()); i++)
			{
				GetChairPicture( chairs[i], frame, chairImg );
				if (!((writers[i])->WriteFrame( &chairImg, 0 )))
				{
					WStrStream s;
					s << L"Failed to write video file, chair id = " << (chairs[i]).index << std::endl;
					PrintMessage( s.str() );
					throw std::runtime_error("");
				}
			} 
		}
		while (res == avideolib::IVideoReader::NORMAL);
	}
	catch (std::runtime_error &)
	{
		ok = false;

		// Remove chair files on failure.
		alib::ClearPtrContainer( writers );
		if (!(fileLst.empty()))
		{
			for (std::list<WStr>::iterator it = fileLst.begin(); it != fileLst.end(); ++it)
				DeleteVideoFile( *it );
		}
	}

	alib::ClearPtrContainer( writers );

	// Remove successfully processed video file.
	pReader.reset(0);
	if (ok)
		ok = DeleteVideoFile( videoFile );

	return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Main function. */
//-------------------------------------------------------------------------------------------------
int _tmain( int argc, TCHAR * argv[], TCHAR * envp[] )
{
	std::wcout.imbue( std::locale ( ".866" ) );

	argc;argv;envp;
	if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
	{
		std::wcout << _T("Fatal Error: MFC initialization failed") << std::endl;
		return 1;
	}

	CFileFind      dirFind, fileFind;
	CString        inFile, outFile;
	WStr           dirPattern, chairPattern, videoPattern, camId, startPath, camPath, videoFile;
	BaseChairExArr chairs;
	BaseChairArr   baseChairs;

	try
	{
		startPath = argv[0];
		startPath = alib::GetFilePath( startPath );
		alib::AddTrailingBackslash( startPath );
		dirPattern += L"cam*";

		// Open log file.
		{
			USES_CONVERSION;
			WStr logName;
			(logName = startPath) += L"VideoToChairs.log";
			g_log.open( W2CA(logName.c_str()), std::ios::out );
		}

		// Scan all subdirectories that has "cam" phrase starting from the program location.
		for (BOOL bDirContinue = dirFind.FindFile( dirPattern.c_str() ); bDirContinue;)
		{
			bDirContinue = dirFind.FindNextFile();
			if (!(dirFind.IsDirectory()))
				continue;

			camId = (LPCWSTR)(dirFind.GetFileName());
			camPath = (LPCWSTR)(dirFind.GetFilePath());
			alib::AddTrailingBackslash( camPath );

			// Find and load corresponding XML file.
			(((chairPattern = startPath) += L"*_") += camId) += L"_chairs.xml";
			if (fileFind.FindFile( chairPattern.c_str() ))
			{
				fileFind.FindNextFile();
				if ((LoadGaugeHallFromFile( (LPCWSTR)(fileFind.GetFilePath()), baseChairs ) != 0) ||
					!(csalgocommon::ExtractChairSubSet( baseChairs, -1, chairs )))
				{
					WStrStream s;
					s << L"Chair file does not exist or corrupted:" << std::endl
						<< (LPCWSTR)(fileFind.GetFilePath()) << std::endl << std::endl;
					PrintMessage( s.str() );					
					chairs.clear();
				}	
				csalgocommon::InitializeChairs( chairs );
			}

			if ( chairs.empty() )
			{
				PrintMessage( L"chairs not loaded" );
				continue;
			}


			// For all files is sudbdirectory ...
			(videoPattern = camPath) += L"*_video.avd";
			for (BOOL bFileContinue = fileFind.FindFile( videoPattern.c_str() ); bFileContinue;)
			{
				// Is normal video file? Does file name contain camera id substring?
				bFileContinue = fileFind.FindNextFile();
				if (!(fileFind.IsNormal()) && !(fileFind.IsArchived()))
					continue;
				videoFile = (LPCWSTR)(fileFind.GetFilePath());
				if (videoFile.find( camId ) == WStr::npos)
					continue;

				// Process video file.
				std::wcout << L"Processing " << videoFile << L" ...";
				SplitOneVideoFile( videoFile, chairs );
				std::wcout << L"finished" << std::endl;
			}
		}
	}
	catch (alib::GeneralError & e)
	{
		std::wcout << e.what() << std::endl;
		return 1;
	}
	return 0;
}

