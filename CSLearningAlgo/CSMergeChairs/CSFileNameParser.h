#pragma once

class CSFileNameParser
{
public:
	CSFileNameParser(void);
	~CSFileNameParser(void);
	int IsCamNN( const std::wstring & str );
	std::wstring GetCamStr( int camNo );
	std::wstring GetVideoFileNamePattern( int camNo );
	std::wstring GetClusterizedFileNamePattern( int camNo );
	
	std::wstring GetMergedFromClusterizedName( const std::wstring & clustName );
	std::wstring GetClusterizedFileNamePattern( int camNo, int chairNo );
	std::wstring GetMergedFileNamePattern( int camNo, int chairNo );
	std::wstring GetRefinedFileNamePattern( int camNo );


	int GetChairNNN( const std::wstring & str );
	std::wstring GetMarkingFileNamePattern( int camNo );
};
