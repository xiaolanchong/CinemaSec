// OleDBConnection.cpp: implementation of the COleDBConnection class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
//#include "csclient.h"
#include "OleDBConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string COleDBConnection::GetErrorMessageA(HRESULT hr)
{
/*	LPSTR lpBuffer;
	if (::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPSTR) &lpBuffer, 0, NULL) == 0)
	{
		return std::string();
	}
	else
	{
		std::string s( lpBuffer );
		LocalFree(lpBuffer);
		return s;
	}*/
	std::string s;
	IErrorInfo* pErrorInfo;
	hr = GetErrorInfo ( 0, &pErrorInfo );
	if( hr == S_OK )
	{
		BSTR bstrDesc;
		hr = pErrorInfo->GetDescription( &bstrDesc);
		if( hr == S_OK )
		{
			USES_CONVERSION;
			LPCSTR szDesc = W2CA( bstrDesc );
			s = szDesc;
		}
		SysFreeString( bstrDesc );
		pErrorInfo->Release();
	}
	return s;
}

void COleDBConnection::TraceError(HRESULT hr)
{
	std::string s = COleDBConnection::GetErrorMessageA(hr);
	s += '\n';
	OutputDebugStringA( s.c_str() );
}

COleDBConnection::COleDBConnection()
{

}

COleDBConnection::~COleDBConnection()
{

}

void	COleDBConnection::OpenDataLinkWindow()
{
	HRESULT hr;
	hr = m_dbDataSource.Open( static_cast<HWND>(NULL) );
	if(FAILED(hr))
	{
		TraceError(hr);
		throw DBConnectionException("Invalid data source");
	}
}

void	COleDBConnection::Open(const std::tstring& strConnectionString, bool bShowWindow) 
{
	USES_CONVERSION;
	LPCWSTR szWideCS = T2CW( strConnectionString.c_str() );
	m_dbSession.Close();
	m_dbDataSource.Close();
	HRESULT hr;
	hr = m_dbDataSource.OpenFromInitializationString( szWideCS ); 
	if( FAILED(hr) )
	{
		TraceError(hr);
		if( bShowWindow )
		{
		
		m_dbDataSource.Close();
		OpenDataLinkWindow();
		}
		else
		{
			throw DBConnectionException("Invalid connection string");
		}
	}
	CComBSTR	str;
	m_dbDataSource.GetInitializationString( &str );

	hr = m_dbSession.Open(m_dbDataSource);
	if( FAILED(hr) )
	{
		TraceError(hr);
		m_dbDataSource.Close();
		m_dbSession.Close();
		throw DBConnectionException("Invalid session");
	}
}

bool	COleDBConnection::CheckConnection()
{
//	VARIANT var;
//	VariantInit(&var);
	CComVariant var;
	HRESULT hr = S_OK;
	if (m_dbDataSource.m_spInit != NULL)
	{
		hr = m_dbDataSource.GetProperty(DBPROPSET_DATASOURCEINFO, DBPROP_CONNECTIONSTATUS, &var);
	}
	else return false;
	if (FAILED(hr) || (var.vt == VT_I4 && var.lVal == DBPROPVAL_CS_COMMUNICATIONFAILURE))
	{
		m_dbSession.Close();
		OutputDebugString ( _T("Database communication lost. Pending reopen...\n") );
		return false;
	}
	return true;
}