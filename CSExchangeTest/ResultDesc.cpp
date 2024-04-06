//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper for description extraction from system & user DLLs
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 01.06.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ResultDesc.h"
//======================================================================================//
//                                   class ResultDesc                                   //
//======================================================================================//
static void ReleaseLibrary( HMODULE* p )
{
	FreeLibrary(*p);
	boost::checked_delete( p );
}

ResultDesc::ResultDesc(LPCWSTR* ppDllNames, size_t nCount)
{
	m_DynLibraries.reserve( nCount );
	for( size_t i = 0; i < nCount ; ++i)
	{
		HMODULE hMod = LoadLibraryEx( ppDllNames[i] , 0, LOAD_LIBRARY_AS_DATAFILE  );
		if( !hMod )
		{
			boost::shared_ptr<HMODULE> pLib ( new HMODULE ) ;
			*pLib = hMod;
			m_DynLibraries.push_back( pLib );
		}
	}
}

ResultDesc::~ResultDesc()
{
}

std::wstring ResultDesc::GetErrorDesc( DWORD dwRes )
{
	DWORD res;
	LPCWSTR pBuffer;
	res = FormatMessageW(	
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, dwRes, 0, (LPWSTR) &pBuffer, 0, 0
		);
	if( res )
	{
		std::wstring s( pBuffer, pBuffer + res );
		LocalFree( (HLOCAL) pBuffer );
		return s;
	}
	
	for( size_t i = 0 ; i < m_DynLibraries.size() ; ++i )
	{
		res = FormatMessageW(	
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_HMODULE,
			*m_DynLibraries[i], dwRes, 0, (LPWSTR) &pBuffer, 0, 0
		);
		if( res )
		{
			std::wstring s( pBuffer, pBuffer + res );
			LocalFree( (HLOCAL)pBuffer );
			return s;
		}
	}
	return std::wstring();
}