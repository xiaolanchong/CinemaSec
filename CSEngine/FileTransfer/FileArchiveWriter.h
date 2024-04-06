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
#ifndef _FILE_ARCHIVE_WRITER_9816656731104056_
#define _FILE_ARCHIVE_WRITER_9816656731104056_

#include "../../CSChair/common/Exception.h"
#include "../../CSChair/debug_int.h"
#include <atlbase.h>
//======================================================================================//
//                               class FileArchiveWriter                                //
//======================================================================================//

MACRO_EXCEPTION(  FileArchiveWriterException,	CommonException )
MACRO_EXCEPTION(  FileCreateException,			FileArchiveWriterException )
MACRO_EXCEPTION(  FileWriteException,			FileArchiveWriterException )

class FileArchiveWriter
{
	MyDebugOutputImpl			m_Debug;
	std::wstring				m_sFileName;
	CHandle						m_File;
	CHandle						m_Event;
public:
	FileArchiveWriter( const std::wstring& sFileName, IDebugOutput* pDbgInt );
	virtual ~FileArchiveWriter();

	void	Write( const void* pData, size_t nSize );
	void	Cancel();
};

#endif // _FILE_ARCHIVE_WRITER_9816656731104056_