//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Implements cyclic html log, gets the max file number in the directory
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 21.03.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CyclicLog.h"
#include <map>
#include <algorithm>
#include <shlwapi.h>
#include <strsafe.h>
//======================================================================================//
//                                   class CyclicLog                                    //
//======================================================================================//

CyclicLog::~CyclicLog()
{
}

CyclicLog::CyclicLog( const wchar_t* szPath, const wchar_t*	szSystem, size_t nFileNumber, size_t nMaxSize ):
	m_nFileNumber( nFileNumber ),
	m_nMaxSize( nMaxSize ),
	m_sSystem( szSystem ),
	m_timeStart( CTime::GetCurrentTime() ),
	m_nFileNo(0)
{
try
{
	WCHAR szBuf[ MAX_PATH ];
	if( StringCchCopyW ( szBuf, MAX_PATH, szPath ) != S_OK )
		throw CyclicLogException( "Too small buffer" );
	if( !PathAppendW( szBuf, szSystem ) )
		throw CyclicLogException( "Too small buffer" );
	PathAddBackslashW( szBuf );
	m_sPath = szBuf;

	CreateLogFile();
}
catch(  CyclicLogException )
{

}
}

bool	CyclicLog::AddRecord( HtmlLog::Severity sev, __time64_t time, const wchar_t* szValue )
{
	if( !m_LogFile ) return false;
	if( m_LogFile->GetSize() > (long) m_nMaxSize )
	{
		// create new file
		CreateLogFile(); 
	}
	m_LogFile->AddRecord( sev, time, szValue );
	return true;
}

void	CyclicLog::CreateLogFile()
{
	m_LogFile.reset();
	SHCreateDirectoryExW( NULL, m_sPath.c_str(), 0 );
	CheckDirectory( m_sPath  );
	__time64_t timeNow = _time64(0);
	// rewrite if no use MFC
	CString sLastTempFile( m_sPath.c_str() + m_timeStart.Format( _T("%b%d %H-%M") ) );
	if( m_nFileNo )
	{
		CString s;
		// like PathYetAnotherMakeUniqueName but don't use already deleted file's names
		s.Format( _T(" (%u)"), m_nFileNo );
		sLastTempFile += s;
	}
	sLastTempFile += _T(".html");
	++m_nFileNo;
	m_LogFile = boost::shared_ptr< HtmlLog >( new HtmlLog( sLastTempFile, m_sSystem.c_str(), timeNow ) );
}

namespace
{

MACRO_EXCEPTION( DeleteLogFileException, CyclicLogException ); 

struct DeleteFilePred
{
	void operator () ( const std::pair< CTime, std::wstring >& p) const
	{
		BOOL res = ::DeleteFileW( p.second.c_str() );
		UNREFERENCED_PARAMETER(res);
#if 0
		if( !res ) throw DeleteLogFileException( "Failed to delete a log file" );
#endif
	}
};

}

void	CyclicLog::CheckDirectory( const std::wstring& sPath )
{
	std::wstring sFullPath( sPath + L"*.html" );
	WIN32_FIND_DATA	fd;
	HANDLE hFind = FindFirstFileW(sFullPath.c_str(), &fd );
	typedef std::map< CTime, std::wstring, std::greater<CTime> >	FileMap_t;
	FileMap_t FileNameMap;
	BOOL res = hFind != INVALID_HANDLE_VALUE;
	while( res )
	{
		CTime time( fd.ftCreationTime );
		std::wstring sName( fd.cFileName );
		FileNameMap.insert( std::make_pair( time, sPath + sName ) );
		res = FindNextFileW( hFind, &fd );
	}
	if( hFind != INVALID_HANDLE_VALUE ) FindClose( hFind );
	FileMap_t::iterator it = FileNameMap.begin( );
	std::advance( it, m_nFileNumber );
	// erase first n file names from map, remaining is the goal
	FileNameMap.erase( FileNameMap.begin(), it );
	try
	{
		std::for_each( FileNameMap.begin(), FileNameMap.end(), DeleteFilePred() );
	}
	catch( CyclicLogException )
	{

	}
}