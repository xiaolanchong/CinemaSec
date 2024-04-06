#include "StdAfx.h"
#include ".\csfilenameparser.h"

CSFileNameParser::CSFileNameParser(void)
{
}

CSFileNameParser::~CSFileNameParser(void)
{
}
int CSFileNameParser::IsCamNN( const std::wstring & str )
{
	if ( str.substr(0, 3) == L"cam" )
	{
		std::wstring numstr = str.substr(3, 2);
		if( iswdigit( numstr[0]) && iswdigit(numstr[1]))
			return _wtoi( numstr.c_str() );
		else
			return -1;
	}
	else
	{
		return -1;
	}
}

std::wstring CSFileNameParser::GetVideoFileNamePattern( int camNo )
{
	// *camNN*_video.avd
	std::wstring spattern = L"*" + GetCamStr( camNo )  + L"*_video.avd";
	return spattern;
}

std::wstring CSFileNameParser::GetClusterizedFileNamePattern( int camNo )
{
	// *camNN*_video.avd
	std::wstring spattern = L"clusterized_" + GetCamStr( camNo )  + L"*.avd";
	return spattern;
}

std::wstring CSFileNameParser::GetRefinedFileNamePattern( int camNo )
{
	// *camNN*_video.avd
	std::wstring spattern = L"refined_" + GetCamStr( camNo )  + L"*.avd";
	return spattern;
}

std::wstring CSFileNameParser::GetMarkingFileNamePattern( int camNo )
{
	//*camNN_chairs.xml
	std::wstring spattern = L"*" + GetCamStr( camNo ) + L"_chairs.xml";
	return spattern;
}

std::wstring CSFileNameParser::GetCamStr( int camNo )
{
	//camNN
	wchar_t num[5];
	swprintf( num, L"%02d", camNo );
	std::wstring str = L"cam" + std::wstring( num );
	return str;
}

std::wstring CSFileNameParser::GetMergedFromClusterizedName( const std::wstring & clustName )
{
	// for ex: clusterized_cam16_31Aug05_12_chair098.avd
	// delete clusterized

	int pos = (int)(clustName.find( L"clusterized_" ));
	std::wstring tmp = clustName.substr( pos + 12 , clustName.length() - pos - 12 );
	// delete time
	tmp.erase( tmp.length() - 16, 3);
	tmp = L"merged_" + tmp;
	return tmp;
}

std::wstring CSFileNameParser::GetClusterizedFileNamePattern( int camNo, int chairNo )
{
	wchar_t num[5];
	swprintf( num, L"%03d", chairNo );
	std::wstring spattern = L"clusterized_" + GetCamStr( camNo )  +L"*chair" + std::wstring(num) + L".avd";
	return spattern;	
}

std::wstring CSFileNameParser::GetMergedFileNamePattern( int camNo, int chairNo )
{
	wchar_t num[5];
	swprintf( num, L"%03d", chairNo );
	std::wstring spattern = L"merged_" + GetCamStr( camNo )  +L"*chair" + std::wstring(num) + L".avd";
	return spattern;	
}

int CSFileNameParser::GetChairNNN( const std::wstring & str )
{
	if ( str.substr( str.length() - 12, 5 ) == L"chair" )
	{
		std::wstring numstr = str.substr( str.length() - 7, 3);
		if( iswdigit( numstr[0]) && iswdigit(numstr[1]) && iswdigit( numstr[2]))
			return _wtoi( numstr.c_str() );
		else
			return -1;
	}
	else
	{
		return -1;
	}
}