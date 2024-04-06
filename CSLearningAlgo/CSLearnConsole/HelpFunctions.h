
bool DeleteVideoFile( const std::wstring & videoFileName )
{
	int i = _wremove( videoFileName.c_str() );
	return true;
}




bool RunProgramAndWait( std::wstring & progName )
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	USES_CONVERSION;
	if( !CreateProcess( NULL,   // No module name (use command line). 
		(LPWSTR)progName.c_str(), // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		CString err;
		err.Format( _T("CreateProcess failed (%d).\n"),  GetLastError() );
		std::wcout << err;
		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return true;
}



bool IsCamDirectory( const std::wstring & dir )
{
	if ( dir.substr(0, 3) == L"cam")
		return true;
	else
        return false;
}
bool IsVideoFile( const std::wstring & name )
{
	if ( name.length() < 10)
		return false;
	if ( name.substr(name.length() - 10, 10) == L"_video.avd")
		return true;
	else
		return false;
}


bool GetWriterName( const std::wstring & videoFileName, int index, std::wstring & writerName)
{
	wchar_t chairNNN[100];
	swprintf(chairNNN, L"chair%03d.avd", index);

	const wchar_t * first = L"cam";
	const wchar_t * second = L"video";

	int index1 = (int)videoFileName.find(first);
	int index2 = (int)videoFileName.find(second);

	

	if (index2 <= index1 || index1 < 0 || index2 < 0 )
		return false;
	writerName = videoFileName.substr(index1, index2 - index1 ) + std::wstring( chairNNN ); 


	return true;
}

bool GetChairPicture( const BaseChairEx & chair, const Arr2ub & frame, Arr2ub & chairPic )
{
	int w = chair.rect.width();
	int h = chair.rect.height();


	int x0 = chair.rect.x1;
	int y0 = chair.rect.y1;

	chairPic.resize( w, h );

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			Vec2i pt( x + x0, y + y0 );
            if ( chair.region.inside( pt ) )
			{
				chairPic(x, y) = frame( pt.x, pt.y );
			}
			else
			{
				chairPic(x, y) = 0;
			}
		}
	}

	return true;
}

bool SplitOneVideoFile( const std::wstring & videoFileName )
{
	std::wstring chairsFileName;
	std::wstring totalVideoFileName;
	std::wstring camstr;

	
	//find chairs file
	{
		const wchar_t * sub = L"cam";

		int index = (int)videoFileName.find(sub);

		if ( index >= 0 )
		{
			camstr = videoFileName.substr( index, 5);
		}

		std::wstring ptrn =L"*" + camstr + L"*chairs.xml";  
		
		CFileFind finder;
		if ( finder.FindFile( ptrn.c_str() ) )
		{
			finder.FindNextFile();
			chairsFileName = (LPCWSTR)finder.GetFilePath();
		}
		else
		{
			throw std::runtime_error( "cannot open chairs file or video file name is incorrect\n" );
		}
	}

	totalVideoFileName = camstr + L"\\" + videoFileName;

	std::auto_ptr < avideolib::IVideoReader > pReader;
	pReader.reset( avideolib::CreateAVideoReader( totalVideoFileName.c_str(), 0, INVERT_AXIS_Y ) );

	if (pReader.get() == 0)
	{
		throw std::runtime_error( "cannot open video file\n" );
	}

	std::vector < BaseChairEx > chairs;
	{ // load chairs in appropriate format
		std::vector < BaseChair > baseChairs;
		LoadGaugeHallFromFile( chairsFileName.c_str(), baseChairs );
		csalgocommon::ExtractChairSubSet( baseChairs, -1, chairs );
		csalgocommon::InitializeChairs( chairs );
	}

	// reset video writers

	std::vector < avideolib::IVideoWriter * > writerPtrArr;
	writerPtrArr.resize( chairs.size(), (avideolib::IVideoWriter *) 0 );
	{
		for (int i = 0; i < (int)writerPtrArr.size(); i++)
		{
			std::wstring writerName;
			GetWriterName( videoFileName, chairs[i].index, writerName );
			writerName = camstr + L"\\" + writerName;
			writerPtrArr[i] = avideolib::CreateAVideoWriter( writerName.c_str(), 0, false, INVERT_AXIS_Y );
		}
	}

	Arr2ub frame;
	int nFrame = 0;
	while ( pReader->ReadFrame( &frame, -1, 0, 0 ) != avideolib::IVideoReader::END_IS_REACHED ) 
	{
		std::wcout << L"frame nuber " <<  nFrame << std::endl;
		for (int i = 0; i < (int)chairs.size(); i++)
		{
			Arr2ub chairPic;
			GetChairPicture( chairs[i], frame, chairPic );
			writerPtrArr[i]->WriteFrame( &chairPic, 0 );
		}	
		nFrame++;
	}

	
	// deleting video writers
	for (int i = 0; i < (int)writerPtrArr.size(); i++)
	{
		delete writerPtrArr[i];
	}

	pReader.reset( 0 );
	DeleteVideoFile( totalVideoFileName );
	return true;
}

bool ProcessCamDirecory( const std::wstring & dir )
{
	CFileFind finder;

	std::wstring ptrn =  dir +  _T("\\*.*");
	BOOL bWorking = finder.FindFile( ptrn.c_str() );
	while ( bWorking ) 
	{
		bWorking = finder.FindNextFile();
		std::wstring file = (LPCTSTR)finder.GetFileName();
        if ( IsVideoFile( file ) )
			SplitOneVideoFile( file );
	}

	return true;
}

