/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Utility.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is Utility class impl
*
*
*/
#include "Common.hpp"
#include "Exception.hpp"
#include "Utility.hpp"

/* Retrieve the CS database connection string */
void CUtility::GetDatabaseConnectionString( std::wstring& sConnectionString )
{
	CRegKey connectionStringKey;
	LONG result;

	//Query registry for the database connection string 
	result = connectionStringKey.Open( HKEY_LOCAL_MACHINE, L"Software\\Elvees\\CinemaSec");
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat(L"Can't open registry key HKLM\\Software\\Elvees\\CinemaSec, error: %1%" ) % result ) ) ;

	TCHAR szValue[512];
	DWORD dwValueLen = sizeof( szValue );
	result = connectionStringKey.QueryValue( L"DBConnectionString", NULL, szValue, &dwValueLen );
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat( L"Can't query value of the DBConnectionString, error: %1%" ) % result ) );

	//Append received value
	sConnectionString.append( szValue, dwValueLen );
}

/* Retrieve the CS archive path */
void CUtility::GetArchivePath( std::wstring& sPath )
{
	CRegKey connectionStringKey;
	LONG result;

	//Query registry for the database connection string 
	result = connectionStringKey.Open( HKEY_LOCAL_MACHINE, L"Software\\Elvees\\CinemaSec");
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat(L"Can't open registry key HKLM\\Software\\Elvees\\CinemaSec, error: %1%" ) % result ) ) ;

	TCHAR szValue[512];
	DWORD dwValueLen = sizeof( szValue );
	result = connectionStringKey.QueryValue( L"ArchivePath", NULL, szValue, &dwValueLen );
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat( L"Can't query value of the ArchivePath, error: %1%" ) % result ) );

	::PathAddBackslash( szValue );
	
	//Append received value
	sPath = szValue;	
}

/* Retrieve room IDs from registry */
void CUtility::GetRoomIDs( std::vector<DWORD>& roomIds  )
{
	CRegKey connectionStringKey;
	LONG result;

	//Query registry for the database connection string 
	result = connectionStringKey.Open( HKEY_LOCAL_MACHINE, L"Software\\Elvees\\CinemaSec");
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat(L"Can't open registry key HKLM\\Software\\Elvees\\CinemaSec, error: %1%" ) % result ) ) ;

	DWORD dwValue[64];
	DWORD dwValueLen = sizeof( dwValue );
	result = connectionStringKey.QueryValue( L"RoomID", NULL, (void*)&dwValue[0], &dwValueLen );
	if( result != ERROR_SUCCESS || dwValueLen == 0  )
		throw CException<CUtility>( boost::str( boost::wformat( L"Can't query value of the RoomID, error: %1%" ) % result ) );

	//Copy received values
	DWORD dwValuesNum =  dwValueLen / sizeof(dwValueLen);
	
	roomIds.resize( dwValuesNum );
	std::copy( &dwValue[0], &dwValue[ dwValuesNum ], roomIds.begin() );
}


/* Retrieve Cinema IDs from registry */
void CUtility::GetCinemaID( DWORD& dwCinemaID  )
{
	CRegKey connectionStringKey;
	LONG result;

	//Query registry for the database connection string 
	result = connectionStringKey.Open( HKEY_LOCAL_MACHINE, L"Software\\Elvees\\CinemaSec");
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat(L"Can't open registry key HKLM\\Software\\Elvees\\CinemaSec, error: %1%" ) % result ) ) ;

	DWORD dwValue;
	DWORD dwValueLen = sizeof( dwValueLen );
	result = connectionStringKey.QueryValue( L"CinemaID", NULL, (void*)&dwValue, &dwValueLen );
	if( result != ERROR_SUCCESS )
		throw CException<CUtility>( boost::str( boost::wformat( L"Can't query value of the CinemaID, error: %1%" ) % result ) );

	//Copy received values
	dwCinemaID = dwValue;
}

/* Get the local host ip */
void CUtility::GetLocalHostIP( std::wstring& sHostIP )
{
	WSADATA wsaData;
	WSAStartup( MAKEWORD(1,1), &wsaData );
 
	char szHostName[1024];
	TCHAR szHostIP[20];
	DWORD dwHostIP = 0;
	if(!gethostname(szHostName, sizeof(szHostName)))
	{
		if(LPHOSTENT lpHostEnt = gethostbyname(szHostName))
		{
			dwHostIP = ((LPIN_ADDR)lpHostEnt->h_addr)->s_addr;
			struct ip_t{ BYTE a,b,c,d;	};

			ip_t* ip = static_cast<ip_t*>((void*)&dwHostIP);
			swprintf( szHostIP, L"%u.%u.%u.%u", ip->a, ip->b, ip->c, ip->d  );

			sHostIP.append( szHostIP );
		}
	}

	//Cleanup WSA on exit
	WSACleanup();
}
//////////////////////////////////////////////////////////////////////////

/* Build relative path like "\19\05-03-03\13-32-00.avi" */
void CUtility::BuildPath( std::wstring& sPath, const std::wstring& sFileExt, 
						  short m_nCamId, short m_nYear, 
						  short m_nMonth, short m_nDay, 
						  short m_nHour, short m_nMinute, short m_nSecond )
{
	assert( false && L"Currently not implemented" );   
}

/* Parse relative path like "\19\05-03-03\13-32-00.avi" */
void CUtility::ParsePath( const std::wstring& sPath, std::wstring& sFileExt, short& m_nCamId, 
					  short& m_nYear, short& m_nMonth, short& m_nDay, short& m_nHour, short& m_nMinute, short& m_nSecond )
{
	assert( false && L"Currently not implemented" );
}


