//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	”правление последовательной записью в файл при получении архива
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.08.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "FileArchiveWriter.h"

//======================================================================================//
//                               class FileArchiveWriter                                //
//======================================================================================//
FileArchiveWriter::FileArchiveWriter(const std::wstring& sFileName, IDebugOutput* pDbgInt ): 
	m_sFileName(sFileName),
	m_Event( CreateEvent(NULL, FALSE, TRUE, NULL ) ),
	m_Debug( pDbgInt ) 
{
	HANDLE h = CreateFileW(
		sFileName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		) ; 
	if( h  == INVALID_HANDLE_VALUE )
	{
		throw FileCreateException("");
	}
	else
	{
		m_File.Attach(h); 
	}

}

FileArchiveWriter::~FileArchiveWriter()
{
}

void	FileArchiveWriter::Write( const void* pData, size_t nSize )
{
#if 0
	WaitForSingleObject( m_Event, 3 * 1000 );
	OVERLAPPED ov;
	memset(&ov, 0, sizeof(OVERLAPPED) );
	ov.hEvent = m_Event;
	DWORD dwWritten;
	BOOL res = WriteFile( m_File, pData, DWORD(nSize), &dwWritten, &ov );
	if( !res || dwWritten != nSize ) throw FileWriteException("");
#else
	DWORD dwWritten;
	BOOL res = WriteFile( m_File, pData, DWORD(nSize), &dwWritten, NULL );
	if( !res || dwWritten != nSize ) throw FileWriteException("");
#endif
}

void	FileArchiveWriter::Cancel()
{
	m_File.Close();
#if 0
	SHFILEOPSTRUCTW op;
	memset( &op, 0, sizeof(SHFILEOPSTRUCTW) );
	op.wFunc	= FO_DELETE;
	op.pFrom	= m_sFileName.c_str();
	op.fFlags	= FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	int res = SHFileOperationW( &op );
	if( !res ) m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to delete %ws", m_sFileName.c_str() );
#else
	BOOL res = DeleteFileW(  m_sFileName.c_str());
	if( !res ) m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to delete %ws", m_sFileName.c_str() );
#endif
}