#include "stdafx.h"
#include ".\nameparse.h"
#include <wininet.h>
#include <shlwapi.h>

#include "../public/memleak.h"

std::tstring UrlName::GetSchema()	const
{
	return GetPart(URL_PART_SCHEME);
}

std::tstring UrlName::GetHostName()	const
{
	return GetPart(URL_PART_HOSTNAME);
}

std::tstring UrlName::GetPort()		const
{
	return GetPart(URL_PART_PORT);
}

std::tstring UrlName::GetPassword()	const
{
	return GetPart(URL_PART_PASSWORD);
}

std::tstring UrlName::GetQuery()		const
{
	return GetPart(URL_PART_QUERY);
}

std::tstring UrlName::GetUserName()	const
{
	return GetPart(URL_PART_USERNAME);
}

std::tstring	UrlName::GetPart(DWORD dwPart) const
{
	TCHAR szBuf[INTERNET_MAX_URL_LENGTH + 1];
	DWORD dwSize = INTERNET_MAX_URL_LENGTH + 1;
	HRESULT hr = UrlGetPart( m_sUrl.c_str(), szBuf, &dwSize, dwPart, 0 );
	return SUCCEEDED(hr)? std::tstring( szBuf ) : std::tstring();
}

NameParse::NameParse( LPCTSTR szVideoSource):
	m_vt(vidt_error)
{
	try
	{
		CLSID cls;
		USES_CONVERSION;
		LPOLESTR szStr = CT2OLE( szVideoSource );
		if( UuidFromStringW( (unsigned short*)szStr, &cls ) == RPC_S_OK ||
			CLSIDFromString( szStr, &cls )	== NOERROR 	)
		{
			m_vt		= vidt_camera ;
			m_sSource	= szVideoSource;
			return;
		}
		else if( PathFileExists( szVideoSource ) )
		{
			m_vt		= vidt_file;
			m_sSource	= szVideoSource ;
			return;  
		}
		else if( UrlIs ( szVideoSource, URLIS_FILEURL ) )
		{
			TCHAR szBuf[ MAX_PATH  ];
			DWORD dwBufSize = MAX_PATH ;
			HRESULT hr = PathCreateFromUrl( szVideoSource, szBuf, &dwBufSize, 0 );
			if( FAILED(hr) ) throw SourceTypeException("Hostname error");
			m_vt		= vidt_file;
			m_sSource	= szBuf ;
			return;  
		}
		else if(  UrlIs ( szVideoSource, URLIS_URL )  )
		{
			TCHAR szBuf[ INTERNET_MAX_URL_LENGTH ];
			DWORD dwBufSize = INTERNET_MAX_URL_LENGTH;
			HRESULT hr = UrlGetPart( szVideoSource, szBuf, &dwBufSize, URL_PART_SCHEME, 0 );
			if( FAILED(hr) || !lstrcmp( szBuf, _T("chsv"))) 
				throw SourceTypeException("Scheme error");
			dwBufSize = INTERNET_MAX_URL_LENGTH;
			hr = UrlGetPart( szVideoSource, szBuf, &dwBufSize, URL_PART_HOSTNAME, 0 );
			if( FAILED(hr)) 
				throw SourceTypeException("Hostname error");
			 m_vt		= vidt_file;
			 m_sSource	= szBuf ; 
			 return;
		}
	}
	catch(SourceTypeException& )
	{
		
	}
}