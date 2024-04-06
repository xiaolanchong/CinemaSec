//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Подготовка и считывание файлов архива
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.08.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "FileArchiveProvider.h"
#include "NetPort.h"
//======================================================================================//
//                              class FakeArchiveProvider                               //
//======================================================================================//

FakeArchiveProvider::FakeArchiveProvider( IDebugOutput* pDbgInt, int nCameraID, INT64 nBeginTime, INT64 nEndTime  ):
	m_nCreateTime ( _time64(0) ),
	m_nReadNo(0),
	IArchiveProvider(pDbgInt),
	m_nFileSize
	(
#ifdef _DEBUG
			15 * 1024 * 1024
#else
			50 * 1024 * 1024
#endif
	)
{
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<cam=%d>Starting FakeArchiveProvider", nCameraID );
}

FakeArchiveProvider::~FakeArchiveProvider()
{
}

bool	FakeArchiveProvider::IsReady()
{
	return ( _time64(0) - m_nCreateTime > 15 );
}

bool	FakeArchiveProvider::Read( DWORD64 nOffset, std::vector<BYTE>& Data, size_t nMaxSize )
{
	const DWORD64 nFileSize = GetFileSize();
	bool bLastPiece = false;
	size_t nFillSize = 0;
	if( nOffset + nMaxSize >= nFileSize )
	{
		nFillSize	= size_t(nFileSize - nOffset);
		bLastPiece	= true;
	}
	else
	{
		nFillSize	= nMaxSize;
		bLastPiece	= false;
	}
	Data.clear();
	Data.resize( nFillSize, m_nReadNo++ );
	if( nFillSize )
	{
		Data[0] = (BYTE)m_nCameraID;
	}
	return bLastPiece;
}

void	FakeArchiveProvider::Cancel()
{
	// cancel op
}

//======================================================================================//
//                              class FileArchiveProvider                               //
//======================================================================================//

FileArchiveProvider::FileArchiveProvider( IDebugOutput* pDbgInt, int nCameraID, INT64 nBeginTime, INT64 nEndTime  ):
	m_nCreateTime ( _time64(0) ),
	m_nOffset(0),
	IArchiveProvider(pDbgInt)
{
	throw SourceException("");
	WCHAR szFileName[MAX_PATH];
	if( SHRegGetPathW(  HKEY_LOCAL_MACHINE, 
						RegSettings::c_szCinemaSecSubkey, L"TransferFile", 
						szFileName, 0 ) != ERROR_SUCCESS )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"<cam=%d>%ws\\TransferFile is not defined", 
			nCameraID, RegSettings::c_szCinemaSecSubkey );
		throw SourceException("");
	}

	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<cam=%d>Starting FileArchiveProvider, open %ws", nCameraID, szFileName );
	HANDLE h = CreateFileW(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		) ; 
	if( h  == INVALID_HANDLE_VALUE )
	{
		throw SourceException("");
	}
	else
	{
		m_File.Attach(h); 
		LARGE_INTEGER Size;
		BOOL res = GetFileSizeEx( h, &Size );
		m_nFileSize = Size.QuadPart;
		if( !res ) throw SourceException("");
	}
}

FileArchiveProvider::~FileArchiveProvider()
{
}

bool		FileArchiveProvider::IsReady()
{
	return ( _time64(0) - m_nCreateTime > 15 );
}

bool		FileArchiveProvider::Read( DWORD64 nOffset, std::vector<BYTE>& Data, size_t nMaxSize )
{
	DWORD dwRead;
	Data.resize( nMaxSize );
	BOOL res = ReadFile( m_File, &Data[0], DWORD(nMaxSize), &dwRead, NULL );
	Data.resize( dwRead );
	m_nOffset += dwRead;
	return m_nOffset == m_nFileSize;
}

void		FileArchiveProvider::Cancel()
{
	m_File.Close();
}

DWORD64		FileArchiveProvider::GetFileSize()  
{ 
	return m_nFileSize;
}