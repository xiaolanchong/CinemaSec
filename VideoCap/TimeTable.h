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
#ifndef _TIME_TABLE_2293249171416996_
#define _TIME_TABLE_2293249171416996_
//======================================================================================//
//                                   class TimeTable                                    //
//======================================================================================//

#include <vector>
#include <map>
#include "Common/ExceptionEx.h"

MACRO_EXCEPTION_EX( TimeTableLoaderException, CommonExceptionEx );
//MACRO_EXCEPTION_EX( TimeTableHolderException, CommonExceptionEx );

namespace XmlLite
{
	class XMLElement;
}

typedef std::vector< std::pair< CTime, CTime > >	CameraTimeTable_t;
typedef std::map< size_t, CameraTimeTable_t >		TimeTable_t;

class TimeTableLoader
{
	TimeTable_t		m_tt;
	typedef std::map< size_t, CString >	PathMap_t;
	PathMap_t							m_PathMap;

	std::pair< size_t, CString>	LoadCamera(  CameraTimeTable_t& CamTT, XmlLite::XMLElement& el ) const; 
	std::pair< CTime, CTime > 	LoadFilm( XmlLite::XMLElement& el ) const ; 
	void						Validate( CameraTimeTable_t& CamTT ) const;
	CString						ValidatePath( CString sPath ) const;
	CTime						ConvertToTime( LPCTSTR szTime ) const;
public:
	enum State
	{
		st_idle,
		st_record
	};

	void	Load( LPCWSTR szFileName ) ; 
	State	GetState( size_t nIndex ) const;
	CString	GetPath( size_t nIndex ) const; 
};

#endif // _TIME_TABLE_2293249171416996_