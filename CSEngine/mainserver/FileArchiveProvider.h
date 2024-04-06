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
#ifndef _FILE_ARCHIVE_PROVIDER_4127412092254334_
#define _FILE_ARCHIVE_PROVIDER_4127412092254334_

#include "../../CSChair/common/Exception.h"
#include "../../CSchair/debug_int.h"
#include <atlbase.h>
//======================================================================================//
//                              class FileArchiveProvider                               //
//======================================================================================//

MACRO_EXCEPTION( FileArchiveProviderException, CommonException )
MACRO_EXCEPTION( SourceException, FileArchiveProviderException )
MACRO_EXCEPTION( ReadException, FileArchiveProviderException )

struct IArchiveProvider
{
protected:
	MyDebugOutputImpl	m_Debug;
public:
	IArchiveProvider( IDebugOutput* pDbgInt) : m_Debug(pDbgInt){}
	virtual bool		IsReady()		= 0;
	virtual bool		Read( DWORD64 nOffset, std::vector<BYTE>& Data, size_t nMaxSize ) = 0;
	virtual void		Cancel()		= 0;
	virtual DWORD64		GetFileSize()	= 0;
};

class FakeArchiveProvider : public IArchiveProvider
{
	int			m_nCameraID;
	INT64		m_nCreateTime;

	BYTE		m_nReadNo;
	DWORD64		m_nFileSize;
	DWORD64		m_nOffset;
public:
	FakeArchiveProvider( IDebugOutput* pDbgInt, int nCameraID, INT64 nBeginTime, INT64 nEndTime );
	virtual ~FakeArchiveProvider();

	virtual bool		IsReady();
	virtual bool		Read( DWORD64 nOffset, std::vector<BYTE>& Data, size_t nMaxSize );
	virtual void		Cancel();
	virtual DWORD64		GetFileSize()  { return m_nFileSize;};
};

class FileArchiveProvider : public IArchiveProvider
{
	int			m_nCameraID;
	INT64		m_nCreateTime;

	DWORD64		m_nFileSize;
	CHandle		m_File;
	DWORD64		m_nOffset;
public:
	FileArchiveProvider( IDebugOutput* pDbgInt, int nCameraID, INT64 nBeginTime, INT64 nEndTime );
	virtual ~FileArchiveProvider();

	virtual bool		IsReady();
	virtual bool		Read( DWORD64 nOffset, std::vector<BYTE>& Data, size_t nMaxSize );
	virtual void		Cancel();
	virtual DWORD64		GetFileSize() ;
};

/*

	IArchiveFile* CreateArchiveFile( int nCameraID, INT64 nBegin, INT64 nEnd );

	struct IArchiveFile
	{
		virtual void Release() = 0;
		virtual bool IsReady() = 0;
		virtual void Cancel()  = 0;
		virtual bool GetFileName( LPCWSTR szFileName, size_t nSize ) = 0;
	};

*/

#endif // _FILE_ARCHIVE_PROVIDER_4127412092254334_