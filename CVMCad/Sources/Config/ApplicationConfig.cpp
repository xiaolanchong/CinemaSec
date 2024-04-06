/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ApplicationConfig.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-04-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Resource.hpp"
#include "Properties.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "VersionNo.h"
#include "WinUI.hpp"

#include "ApplicationConfig.hpp"

CApplicationConfig* CApplicationConfig::m_pSelf = 0;
CSingletonDestroyer<CApplicationConfig> CApplicationConfig::m_destroyer;

CApplicationConfig* CApplicationConfig::GetInstance()
{
	if( m_pSelf == 0 )
	{
		m_pSelf = new CApplicationConfig();
		m_destroyer.SetDestroyable( m_pSelf );
	}

	return m_pSelf;
}


CApplicationConfig::CApplicationConfig()
{
	wstring sFileName( 100, L'' );
	LPVOID pVersionData = NULL;
	DWORD dwLen;
	
	::GetModuleFileName( NULL, &sFileName[0],  (DWORD)sFileName.size() );
	dwLen = ::GetFileVersionInfoSize( sFileName.c_str(), NULL );

	vector<char> buffer( dwLen );
	::GetFileVersionInfo( sFileName.c_str(), NULL, dwLen, &buffer[0] );

	UINT nLength;
	VS_FIXEDFILEINFO* vs_ffi;
	::VerQueryValue( &buffer[0], L"\\", (void**)&vs_ffi, &nLength );

	wstring sVersionStr( 100, L'' );
	if( nLength == sizeof( VS_FIXEDFILEINFO ) )
	{
		DWORD dwMSHigh = vs_ffi->dwFileVersionMS>>16;
		DWORD dwMSLow  = vs_ffi->dwFileVersionMS & 0x0000ffff;;
		DWORD dwLSHigh = vs_ffi->dwFileVersionLS>>16;
		DWORD dwLSLow =  vs_ffi->dwFileVersionLS & 0x0000ffff;

		swprintf( &sVersionStr[0], L" %u.%u.%u.%u", 
			dwMSHigh,dwMSLow, dwLSHigh, dwLSLow );
	}

	m_sAppName = L"CVMCad";
	m_sAppTitle = m_sAppName + sVersionStr;
	m_sDefFileName = L"Untitled.cvm";
}

CApplicationConfig::~CApplicationConfig()
{

}

void CApplicationConfig::GetAppName( wstring& s )
{
	s = m_sAppName;
}

void CApplicationConfig::GetAppTitle( wstring& s )
{
	s = m_sAppTitle;
}

void CApplicationConfig::GetDefFileName( wstring& s )
{
	s = m_sDefFileName;
}

