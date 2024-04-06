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
#ifndef _CYCLIC_LOG_7956605058336955_
#define _CYCLIC_LOG_7956605058336955_

#include "../../CSChair/COMMON/Exception.h"
#include "HtmlLog.h"
#include <boost/shared_ptr.hpp>

MACRO_EXCEPTION( CyclicLogException, CommonException ); 

//======================================================================================//
//                                   class CyclicLog                                    //
//======================================================================================//

class CyclicLog
{
	std::wstring	m_sPath;
	std::wstring	m_sSystem;
	CTime			m_timeStart;
//	CString			m_sLastTempFile;
	DWORD			m_nFileNo;

	size_t			m_nFileNumber;
	size_t			m_nMaxSize;
	void	CheckDirectory( const std::wstring& sPath );
	void	CreateLogFile();
	boost::shared_ptr< HtmlLog >	m_LogFile;
public:
	CyclicLog();
	virtual ~CyclicLog();

	CyclicLog( const wchar_t*	szPath, const wchar_t*	szSystem, size_t nFileNumber, size_t nMaxSize );
	bool	AddRecord( HtmlLog::Severity sev, __time64_t time, const wchar_t* szValue );
};

#endif // _CYCLIC_LOG_7956605058336955_