//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper for loading xml/text resources from resources
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 06.02.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "HtmlResourceHelper.h"

//#include <msxml2.h>
#include <vector>
#include <algorithm>
//======================================================================================//
//                              struct HtmlResourceHelper                               //
//======================================================================================//
HtmlResourceHelper::HtmlResourceHelper()
{
}

HtmlResourceHelper::~HtmlResourceHelper()
{
}

IStream* HtmlResourceHelper::LoadToStream( LPCTSTR szName, LPCTSTR szResourceType )
{
	HMODULE hMod = AfxGetInstanceHandle();
	HRSRC hRes = FindResource( hMod, szName, szResourceType);
	ASSERT(hRes);
	HGLOBAL hMem = LoadResource( 0, hRes );  
	ASSERT(hMem);
	DWORD dwSize	= SizeofResource( hMod, hRes);
	void* pRes		= LockResource( hMem );
	LPCSTR szXml = (const char*)pRes;
	UNREFERENCED_PARAMETER(szXml);
	IStream * pStream;
	HRESULT hr;
	hr = CreateStreamOnHGlobal( 0, TRUE, &pStream );
	ASSERT(hr == S_OK);
	ASSERT(pStream);
	hr = pStream->Write( pRes, dwSize, 0 );
	ASSERT(hr == S_OK);
	LARGE_INTEGER li = {0};
	hr = pStream->Seek( li, STREAM_SEEK_SET, NULL);
	ASSERT(hr == S_OK);
	UnlockResource(hMem);
	FreeResource( hMem );
	return pStream;
}

bool	HtmlResourceHelper::InjectCSSIntoHTML(std::wstring& sHtml, const std::wstring& sCss)
{
	std::wstring sStyleTag( L"</style>" );
	std::wstring::const_iterator it = 
		std::search( sHtml.begin (), sHtml.end (), 
		sStyleTag.begin (), sStyleTag.end () );
	if( it == sHtml.end() ) return false;
	ptrdiff_t nDiff = it - sHtml.begin();
	sHtml.insert( nDiff , sCss );

	return true;
}

std::wstring HtmlResourceHelper::LoadTextResource( LPCTSTR szName, LPCTSTR szResourceType )
{
	std::wstring sText;
	HMODULE hMod = AfxGetInstanceHandle();
	HRSRC hRes = FindResource( hMod, szName, szResourceType);
	ASSERT(hRes);
	HGLOBAL hMem = LoadResource( 0, hRes );  
	ASSERT(hMem);
	DWORD dwSize	= SizeofResource( hMod, hRes);
	void* pRes		= LockResource( hMem );
	LPCSTR szText = (const char*)pRes;

	std::string sAnsiText( szText, szText + dwSize );
	size_t InitSize = mbstowcs(0, sAnsiText.c_str(), dwSize);
	std::vector<WCHAR>	Tmp ( InitSize, L'\0');
	mbstowcs(&Tmp[0], sAnsiText.c_str(), dwSize);
	UnlockResource(hMem);
	FreeResource( hMem );

	return std::wstring( Tmp.begin(), Tmp.end() );
}