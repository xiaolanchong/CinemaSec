/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Storage.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exception.hpp"
#include "Storage.hpp"


//Path structure
// 00\01-02-03\01-02-03.avi == 20B
boost::wregex CCinemaStorage::m_PartialPathExpression( L"(\\d+)\\\\(\\d{2})-(\\d{2})-(\\d{2})\\\\(\\d{2})-(\\d{2})-(\\d{2})\\.avi$" );
//////////////////////////////////////////////////////////////////////////
CCinemaStorage::CFileProxy::CFileProxy( const std::wstring& sPath )
{
	boost::wcmatch what;
	if( 0 != boost::regex_search( sPath.c_str(),  what, m_PartialPathExpression ) )
	{
		m_nCamId =	_wtoi(what[1].str().c_str() );
		m_nYear =	_wtoi(what[2].str().c_str() );
		m_nMonth =	_wtoi(what[3].str().c_str() );
		m_nDay =	_wtoi(what[4].str().c_str() );
		m_nHour =	_wtoi(what[5].str().c_str() );
		m_nMinute = _wtoi(what[6].str().c_str() );
		m_nSecond = _wtoi(what[7].str().c_str() );
	}
}

/* Build path from ints */
void CCinemaStorage::CFileProxy::GetPartialFilePath( std::wstring& sPath )
{
	sPath = boost::str( boost::wformat( L"%d\\%02d-%02d-%02d\\%02d-%02d-%02d.avi" ) 
		% m_nCamId		//Camera ID
		% m_nYear		//Year
		% m_nMonth		//Month
		% m_nDay		//Day
		% m_nHour		//Hour
		% m_nMinute		//Minute
		% m_nSecond		//Second
		);

}

DATE CCinemaStorage::CFileProxy::GetDate()
{
	return static_cast<DATE>( GetOleDateTime() );
}

COleDateTime CCinemaStorage::CFileProxy::GetOleDateTime()
{
	return COleDateTime( m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond );
}



void CCinemaStorage::CFileProxy::GetPartialPath( std::wstring& sPath )
{
	sPath = boost::str( boost::wformat( L"%d\\%02d-%02d-%02d" ) 
		% m_nCamId		//Camera ID
		% m_nYear		//Year
		% m_nMonth		//Month
		% m_nDay		//Day
		);
}

bool CCinemaStorage::CFileProxy::operator<( const CFileProxy& rhs )
{
	//Year
    if( m_nYear < rhs.m_nYear )
		return true;
	else if( m_nYear > rhs.m_nYear )
		return false;

	//Month
	else if( m_nMonth < rhs.m_nMonth )
		return true;
	else if( m_nMonth > rhs.m_nMonth )
		return false;

	//Day
	else if( m_nDay < rhs.m_nDay )
		return true;
	else if( m_nDay > rhs.m_nDay )
		return false;
	
	//Hour
	else if( m_nHour < rhs.m_nHour )
		return true;
	else if( m_nHour > rhs.m_nHour )
		return false;

	//Minute
	else if( m_nMinute < rhs.m_nMinute )
		return true;
	else if( m_nMinute > rhs.m_nMinute )
		return false;

	//Second
	else if( m_nSecond < rhs.m_nSecond )
		return true;
	
    return false;
}

bool CCinemaStorage::CFileProxy::operator==( const CFileProxy& rhs )
{
	return 
		(( m_nCamId == rhs.m_nCamId )
		&&( m_nYear == rhs.m_nYear )
		&&( m_nMonth == rhs.m_nMonth )
		&&( m_nDay == rhs.m_nDay )
		&&( m_nHour == rhs.m_nHour )
		&&( m_nMinute == rhs.m_nMinute )
		&&( m_nSecond == rhs.m_nSecond ) ) ? true : false;
}

bool CCinemaStorage::CFileProxy::operator!=( const CFileProxy& rhs )
{
	return !operator==( rhs );
}
//////////////////////////////////////////////////////////////////////////
CCinemaStorage::CCinemaStorage( const std::wstring& sPath, std::vector<int>& nCamIds, IDebugOutput* pDebug )
{
	assert( pDebug );
	m_pDebug = pDebug;

	m_nCamIds = nCamIds;
	m_sPath = sPath;

	//Storage isn't touched
	m_bStorageModified = false;
}

//////////////////////////////////////////////////////////////////////////
/* Get total disk space for the current user */
UINT64 CCinemaStorage::GetTotalSpace()
{
	UINT64 totalNumberOfBytes;
	if( !GetDiskFreeSpaceEx( m_sPath.c_str(), NULL, (PULARGE_INTEGER)&totalNumberOfBytes, NULL ) )
		throw CException<CCinemaStorage>( boost::str( boost::wformat( L"Can't retrieve total disk space, error: %1%" ) % GetLastError() ) );

    return totalNumberOfBytes;
}

/* Get available disk space for the current user */
UINT64 CCinemaStorage::GetAvailableSpace()
{
	UINT64 freeBytesAvailable;
	if( !GetDiskFreeSpaceEx( m_sPath.c_str(), (PULARGE_INTEGER)&freeBytesAvailable, NULL, NULL ) )
		throw CException<CCinemaStorage>( boost::str( boost::wformat( L"Can't retrieve available disk space, error: %1%" ) % GetLastError() ) );

	return freeBytesAvailable;
}

//Get free/total space ratio in percent
double CCinemaStorage::GetAvailableSpaceBalance()
{
	return 100.0f * static_cast<double>(GetAvailableSpace()) / static_cast<double>( GetTotalSpace() );
}
//////////////////////////////////////////////////////////////////////////
/* Sort proxies in the vector */
void CCinemaStorage::Sort()
{
	m_Files.sort();
}

/* Return files count appended to storage*/
size_t CCinemaStorage::GetCount()
{
	return m_Files.size();
}

/* Physically delete the file */
CCinemaStorage::CFileProxyIt CCinemaStorage::Delete( CFileProxyIt fileIt )
{
	//Build the file path
	std::wstring sPath;
	fileIt->GetPartialFilePath( sPath );

	if( !DeleteFile( (m_sPath + sPath).c_str() ) )
		throw CException<CCinemaStorage>( boost::str( boost::wformat( L"Can't delete %1%, error: %2%" ) % sPath % GetLastError() ) );

	//Now storage is modified
	m_bStorageModified = true;

	//Delete the file proxy
	return m_Files.erase( fileIt );
}

/* Fake version of the Delete, use it for test purpose */
CCinemaStorage::CFileProxyIt CCinemaStorage::FakeDelete( CFileProxyIt fileIt )
{
	//Build the file path
	std::wstring sPath;
	fileIt->GetPartialFilePath( sPath );

	m_pDebug->PrintW( IDebugOutput::MessageType::mt_error, (m_sPath + sPath).c_str() );

	//Now storage is modified
	m_bStorageModified = true;

	//Delete the file proxy
	return m_Files.erase( fileIt );
}

CCinemaStorage::CFileProxyIt CCinemaStorage::Begin()
{
	return m_Files.begin();
}

CCinemaStorage::CFileProxyIt CCinemaStorage::End()
{
	return m_Files.end();
}

void CCinemaStorage::GetPath( std::wstring& sPath )
{
	sPath = m_sPath;
}


/* Synchronize disk content with the internal CCinemaStorage content */
void CCinemaStorage::Synchronize()
{
	//Clear file proxies
	m_Files.clear();
	m_Dirs.clear();

	WIN32_FIND_DATA wfd;
	HANDLE hFindFile_file = NULL, hFindFile_dir = NULL;

	//Iterate through the cameras ids
	for( std::vector<int>::iterator it = m_nCamIds.begin(); it != m_nCamIds.end(); ++it )
	{
		//Build search string, to find directories ( D:\CinemaSec\19\yy-mm-dd )
		std::wstring sFindFileDir = boost::str( boost::wformat( L"%s%d\\\?\?-\?\?-\?\?" ) % m_sPath % (*it) );

		//Iterate through the dir
		BOOL bHasMoreDirs = TRUE;
		hFindFile_dir = FindFirstFile( sFindFileDir.c_str(), &wfd );
		while( INVALID_HANDLE_VALUE != hFindFile_file  && bHasMoreDirs )
		{
			m_Dirs.push_back( boost::str( boost::wformat( L"%s%d\\%s" ) % m_sPath % (*it) % wfd.cFileName ) );
			bHasMoreDirs = FindNextFile( hFindFile_dir, &wfd );
		}
		DWORD dwError = GetLastError();
		if (dwError == ERROR_NO_MORE_FILES) 
			FindClose( hFindFile_dir );
	}

	//Iterate through the cameras ids\yy-mm-dd
	for( std::list<std::wstring>::iterator it = m_Dirs.begin(); it != m_Dirs.end(); ++it )
	{
		//Build search string, to find files ( D:\CinemaSec\19\yy-mm-dd\\hh-mm-ss.avi )
		std::wstring sFindFileDir = boost::str( boost::wformat( L"%s\\\?\?-\?\?-\?\?.avi" ) % (*it) );

		//Iterate through the dir
		BOOL bHasMoreFiles = TRUE;
		hFindFile_file = FindFirstFile( sFindFileDir.c_str(), &wfd );
		while( INVALID_HANDLE_VALUE != hFindFile_file &&  bHasMoreFiles )
		{
			m_Files.push_back( CCinemaStorage::CFileProxy( (boost::str( boost::wformat( L"%s\\%s" ) % (*it) % wfd.cFileName ) ) ) ); 
			bHasMoreFiles = FindNextFile( hFindFile_file, &wfd );
		}
		DWORD dwError = GetLastError();
		if (dwError == ERROR_NO_MORE_FILES) 
			FindClose( hFindFile_file );
	}

}

bool CCinemaStorage::StorageModified()
{
	return m_bStorageModified;
}

void CCinemaStorage::RemoveDirectories()
{
	for( std::list< std::wstring >::iterator it = m_Dirs.begin(); it != m_Dirs.end();  )
	{
		if( RemoveDirectory( it->c_str() ) )
			it = m_Dirs.erase( it );
		else
			++it;
	}

}