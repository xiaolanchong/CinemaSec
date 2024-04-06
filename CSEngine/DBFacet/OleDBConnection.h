// OleDBConnection.h: interface for the COleDBConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLEDBCONNECTION_H__DEFB3BBB_A6E4_48C4_B997_4495019A1365__INCLUDED_)
#define AFX_OLEDBCONNECTION_H__DEFB3BBB_A6E4_48C4_B997_4495019A1365__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CSChair/common/Exception.h"
#include "../../CSChair/common/tstring.h"
#include "DBHelper.h"

MACRO_EXCEPTION(DBMustReconnectException,	DBConnectionException)

class COleDBConnection  
{
protected:
//	std::wstring m_sCachedCS
	CDataSource m_dbDataSource;
	CSession	m_dbSession;

	void			OpenDataLinkWindow();
	std::tstring	GetConnectionString()
	{
		CComBSTR	str;
		m_dbDataSource.GetInitializationString( &str );
		USES_CONVERSION;
		LPCWSTR toT = W2CT( str );
		return std::tstring(toT);
	}
	std::string GetErrorMessageA(HRESULT hr);
	void		TraceError(HRESULT hr);

public:
	COleDBConnection();
	virtual ~COleDBConnection();

	void	Open(const std::tstring& strConnectionString, bool bShowWindow) ;
	bool	CheckConnection();
};

#endif // !defined(AFX_OLEDBCONNECTION_H__DEFB3BBB_A6E4_48C4_B997_4495019A1365__INCLUDED_)
