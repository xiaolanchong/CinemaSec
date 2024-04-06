//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		OLE DB database helper class for 'Timetable ' table access
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   12.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __TIME_TABLE_H_INCLUDED_6724783155017142__
#define __TIME_TABLE_H_INCLUDED_6724783155017142__

#include "DBRows.h"
#include "DBHelper.h"
#include "OleDBConnection.h"


//=====================================================================================//
//                                   class TimeTable                                   //
//=====================================================================================//
class TimeTable :	protected virtual COleDBConnection,
					protected DBTableTimetableFacet
{
	bool	QueryFilm( const std::wstring& sQuery,  FilmTime& time, std::wstring& Name );
	void	CheckConnection()
	{
		if( !m_dbSession.m_spOpenRowset ) throw TableException("Time table connection error");
	}
	void	ThrowDBError( size_t nIndex );
	void	ThrowDBErrorWithAbort( HRESULT hr, size_t i );
public:
	virtual void			SetTimetable(	int nRoomID, 
											const FilmTime& Range, const TimeTable_t& Timetable,
											const std::vector<std::wstring>& Names) 	;
	virtual void			GetTimetable(	int nRoomID, 
											const FilmTime& Range, TimeTable_t& Timetable,
											std::vector<std::wstring>& Names) ;

	virtual bool			GetCurrentOrNextFilmTime( int nRoomID, FilmTime& Time);
	virtual bool			GetCurrentFilmTime( int nRoomID, FilmTime& time, std::wstring& Name);

	virtual bool			GetFilmFromId( int nFilmID, FilmTime& Time, std::wstring& Name   ); 

	virtual void			CheckExistance( const std::wstring& sCinemaName, int nRoomNo, 
											const std::vector<int>& ExchangeID, 
											CTime timeBegin, CTime timeEnd ); // throw NoSuchExchangeID
	virtual void			SetTimetableSmart(	const std::wstring& sCinemaName,
												int nRoomNo,
												const FilmTime& Range,const TimeTable_t& Timetable, 
												const std::vector<std::wstring>& Names,
												const std::vector<int>& ExchangeID) 	; // throws IntervalIntersectionException

	virtual bool			GetPreviousCinemaFilm( int nRoomID, CTime& timeEnd );
	
};

#endif //__TIME_TABLE_H_INCLUDED_6724783155017142__