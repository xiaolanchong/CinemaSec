//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Timetable loader for video capturing
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 25.04.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "TimeTable.h"
#include <algorithm>

#include "xml/xmllite.h"
//======================================================================================//
//                                   class TimeTableLoader                                    //
//======================================================================================//

#define ELEM_ROOT		L"VCTimetable"
#define ELEM_CAMERA		L"Camera"
#define ELEM_FILM		L"Film"

#define	ATTR_ID			L"id"
#define ATTR_BEGIN		L"begin"
#define ATTR_END		L"end"
#define ATTR_PATH		L"path"

void	TimeTableLoader::Load( LPCWSTR szFileName )
try
{
	m_tt.clear();
	XmlLite::XMLDocument xmlDoc;
	xmlDoc.LoadFromFile( szFileName );
	XmlLite::XMLElement elRoot(xmlDoc.GetRoot());

	XmlLite::XMLElement::iterator it = elRoot.begin();
	CameraTimeTable_t CamTT;
	for( ; it != elRoot.end(); ++it)
	{
		XmlLite::XMLElement el(*it);
		std::pair< size_t, CString> p = LoadCamera( CamTT, el );
		m_tt[ p.first ]			= CamTT;
		m_PathMap [ p.first ]	= ValidatePath( p.second ); 
	}
}
catch ( XMLException ) 
{
	throw TimeTableLoaderException( L"Error occuried while loading file" );
};

std::pair< size_t, CString>		TimeTableLoader::LoadCamera(  CameraTimeTable_t& CamTT, XmlLite::XMLElement& el ) const
{
	CamTT.clear();
	int nId;
	std::tstring sPath;
	el.GetAttr( ATTR_ID, nId );
	try
	{
		el.GetAttr( ATTR_PATH, sPath );
	}
	catch ( XMLException ) 
	{
		sPath.clear();
	}	
	XmlLite::XMLElement::iterator it = el.begin();
	CamTT.reserve( 100 ); // condider much enough
	for( ; it != el.end(); ++it)
	{
		XmlLite::XMLElement el(*it);
		std::pair< CTime, CTime > p = LoadFilm( el );
		CamTT.push_back( p );
	}	
	Validate( CamTT );
	return std::make_pair( size_t(nId - 1), CString( sPath.c_str() ) ) ;
}

std::pair< CTime, CTime > 	TimeTableLoader::LoadFilm( XmlLite::XMLElement& el ) const
{
	std::tstring sBegin, sEnd;
	el.GetAttr( ATTR_BEGIN, sBegin );
	el.GetAttr( ATTR_END, sEnd );

	CTime	timeBegin	= ConvertToTime( sBegin.c_str() ),
			timeEnd		= ConvertToTime( sEnd.c_str() );

	CString sDesc;
	if( timeEnd <= timeBegin ) 
	{
		sDesc.Format( _T("Invalid time period - [%s, %s]"), sBegin.c_str(), sEnd.c_str());
		throw TimeTableLoaderException(sDesc);
	}
	return std::make_pair( timeBegin, timeEnd );
}

CTime	TimeTableLoader::ConvertToTime( LPCTSTR szTime ) const
{
	CComVariant var( szTime);
	HRESULT hr;
	CString sDesc; 
	hr = var.ChangeType( VT_DATE );
	if( hr != S_OK ) 
	{
		sDesc.Format( _T("Invalid date/time format - %s"), szTime);
		throw TimeTableLoaderException(sDesc);
	}

	SYSTEMTIME st;
	if( FALSE == VariantTimeToSystemTime( var.date, &st ) )
	{
		sDesc.Format( _T("Failed to convert variant time to SYSTEMTIME"));
		throw TimeTableLoaderException(sDesc);
	}
	else
	{
		if( st.wYear < 2005 )
		{
			// date is invalid
			CTime time = CTime::GetCurrentTime();
			return CTime(	time.GetYear(), time.GetMonth(), time.GetDay(), 
							st.wHour, st.wMinute, 0 );
		}
		else
			return CTime(st);
	}
}

struct Compare1st
{
	bool operator() (	const std::pair<CTime, CTime>& fst, 
						const std::pair<CTime, CTime>& snd ) const 
	{
		return fst.first < snd.first;
	}
};

struct Compare1st_with_2nd
{
	bool operator() (	const std::pair<CTime, CTime>& fst, 
		const std::pair<CTime, CTime>& snd ) const
	{
		return fst.second > snd.first;
	}
};

struct Find_between
{
	CTime	m_Time;
	Find_between( CTime time ) : m_Time( time ){}

	bool operator() (	const std::pair<CTime, CTime>& fst ) const
	{
		return fst.first <= m_Time && m_Time < fst.second ;
	}
};

void	TimeTableLoader::Validate( CameraTimeTable_t& CamTT ) const
{
	std::sort( CamTT.begin(), CamTT.end(), Compare1st() );
	CameraTimeTable_t::iterator itTT = std::adjacent_find( CamTT.begin(), CamTT.end(), Compare1st_with_2nd() );
	if( itTT != CamTT.end() )
	{
		CString sDesc;
		sDesc.Format( _T("Some time periods intersect"));
		throw TimeTableLoaderException(sDesc);
	}
}

TimeTableLoader::State	TimeTableLoader::GetState( size_t nIndex ) const
{
	TimeTable_t::const_iterator itCam = m_tt.find( nIndex );
	if( m_tt.end() == itCam )
	{
		return st_idle;
	}
	CameraTimeTable_t::const_iterator itCamTT = 
		std::find_if( itCam->second.begin(), itCam->second.end(), 
		Find_between( CTime::GetCurrentTime() ) );
	return ( itCamTT != itCam->second.end() ) ? st_record : st_idle ;
}

CString		TimeTableLoader::ValidatePath( CString sPath ) const
{
	if( sPath.IsEmpty() )
	{
		TCHAR szBuf[MAX_PATH];
		VERIFY( GetModuleFileName( GetModuleHandle( NULL ), szBuf, MAX_PATH ) != 0);
		VERIFY( PathRemoveFileSpec( szBuf ) );
		VERIFY( PathAddBackslash( szBuf ) );
		return CString(szBuf);
	}
	else
	{
		CString sDesc;
		BOOL bRes;
		bRes = PathIsDirectory( sPath);
		if( bRes ) return sPath;
		int nRes = SHCreateDirectoryEx( 0, sPath, NULL);
		if( nRes != ERROR_SUCCESS			&&
			nRes != ERROR_ALREADY_EXISTS	&&
			nRes != ERROR_FILE_EXISTS			)
		{
			sDesc.Format( _T("Bad directory name - %s"), (LPCTSTR)sPath );
			throw TimeTableLoaderException ( sDesc );
		}
		else
		{
			bRes = PathIsDirectory( sPath);
			if( bRes ) return sPath;
			else
			{
				sDesc.Format( _T("%s is not a directory name"), (LPCTSTR)sPath );
				throw TimeTableLoaderException ( sDesc );
			}
		}
		return sPath;
	}
}

CString	TimeTableLoader::GetPath( size_t nIndex ) const
{
	PathMap_t::const_iterator it = m_PathMap.find( nIndex );
	if( it == m_PathMap.end() )
	{
		ASSERT(FALSE);
		return CString();
	}
	else
	{
		return it->second;
	}
}	