//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Timetable input message from BoxOffice
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 07.06.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "TimeTableMessage.h"
#include "MessageProcessor.h"
#include "../DBFacet/CinemaOleDB.h"
#include <boost/tuple/tuple.hpp>
#include "../res/en/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//======================================================================================//
//                                class TimeTableMessage                                //
//======================================================================================//

void	BO_TimeTable::Dump(MyDebugOutputImpl& Debug) const 
{
	Debug.PrintW( IDebugOutput::mt_debug_info, L"<-- TIMETABLE -->" );
	Debug.PrintW( IDebugOutput::mt_debug_info, L"msg id = %d", GetID() );
	Debug.PrintW( IDebugOutput::mt_debug_info, L"cinema name = %s", GetCinemaName().c_str() );
	CStringW sBegin	= m_timeBegin.Format( _T("%d.%m.%Y %H:%M")  );
	CStringW sEnd	= m_timeEnd.Format( _T("%d.%m.%Y %H:%M")  );
	Debug.PrintW( IDebugOutput::mt_debug_info, L"from %s to %s", (LPCWSTR)sBegin, (LPCWSTR)sEnd );

	FilmMap_t::const_iterator it = m_Films.begin();
	for( ; it != m_Films.end(); ++it )
	{
		int nRoomNo = it->first;
		const std::vector<BO_TimeTable::Item>& ItemArr = it->second;
		Debug.PrintW( IDebugOutput::mt_debug_info, L"RoomNo=%d", nRoomNo );
		for( size_t i = 0; i < ItemArr.size(); ++i )
		{
			const BO_TimeTable::Item& _Item = ItemArr[i];
			CStringW	sBegin	= !_Item.m_Begin ? L"" : _Item.m_Begin.get_ptr()->Format( _T("%b%d %H:%M")  );
			CStringW	sEnd	= !_Item.m_End ? L"" : _Item.m_End.get_ptr()->Format( _T("%b%d %H:%M")  );
			LPCWSTR		szName	= !_Item.m_Name? L"" :_Item.m_Name.get_ptr()->c_str()  ;

			Debug.PrintW( IDebugOutput::mt_debug_info, L"FilmId=%d, b=%s e=%s name=%s",
				_Item.m_FilmID, (LPCWSTR)sBegin, (LPCWSTR)sEnd, szName );
		}
	}
}


#if 0

static std::wstring Desc_InvalidFilmTime( int nFilmID, CTime timeBegin, CTime timeEnd )
{
	CStringW sBegin = timeBegin.Format( L"%d.%m.%y %H:%M" );
	CStringW sEnd	= timeEnd.Format( L"%d.%m.%y %H:%M" );
	//WCHAR szBuf[255];
	CStringW sDesc;
	sDesc.Format( LoadStringInternal(IDS_EE_FILM_TIME), nFilmID, sBegin, sEnd );
	return std::wstring( sDesc );
}

static std::wstring Desc_InvalidFilmTime( int _1st, int _2nd )
{
	CStringW sDesc;
	sDesc.Format( LoadStringInternal(IDS_EE_INTERSECT2), _1st, _2nd );
	return std::wstring( sDesc );
}

static std::wstring Desc_InvalidFilmTimeInWholePeriod( int nFilmID )
{
	CStringW sDesc;
	sDesc.Format( LoadStringInternal(IDS_EE_WHOLE), nFilmID );
	return std::wstring( sDesc );
}

static std::wstring Desc_InvalidFilmID( int nID )
{
	CStringW sDesc;
	sDesc.Format( LoadStringInternal(IDS_EE_USED), nID );
	return std::wstring( sDesc );
}

#endif
/// end

namespace
{
	typedef boost::tuple< int, CTime, CTime >	FilmTuple_t;

	// FIXME in the one place
	struct CompareFirst
	{
		bool operator ()(const FilmTuple_t& t1, const FilmTuple_t& t2)
		{
			return boost::get<1>(t1) < boost::get<1>(t2);
		}
	};
	struct CompareFirstAndSecond
	{
		bool operator ()(const FilmTuple_t& t1, const FilmTuple_t& t2)
		{
			// NOTE if u change it? change int db trigger too !!!
			return		boost::get<2>(t1) >= boost::get<1>(t2);
			//return t1.m_timeEnd >= t2.m_timeBegin;
		}
	};
}

void		BO_TimeTable::Validate() const
{
	FilmMap_t::const_iterator it = m_Films.begin();
	std::set< int >	AllFilmIDs;
	for( ; it != m_Films.end(); ++it )
	{
		int nRoomNo = it->first;

		std::vector< FilmTuple_t > AllFilmsInRoom; 
		const std::vector<BO_TimeTable::Item>& ItemArr = it->second;
		for( size_t i = 0; i < ItemArr.size(); ++i )
		{
			const BO_TimeTable::Item& _Item = ItemArr[i];
			std::pair < std::set< int >::iterator, bool> p = AllFilmIDs.insert( _Item.m_FilmID );
			if( !p.second )
			{
				throw NoFilmException
					( 
					 GetID()
					);
			}
			bool bValidate = ! (!_Item.m_Begin || !_Item.m_End );
			if( bValidate )
			{
				CTime timeBegin = *_Item.m_Begin.get_ptr();
				CTime timeEnd	= *_Item.m_End.get_ptr();
				if( timeBegin >= timeEnd )	
					throw InvalidDateTimeException( 
					  GetID()
					);
				else if(	timeBegin <		GetBegin()	|| timeEnd < GetBegin() ||
							timeBegin >=	GetEnd()	|| timeEnd >= GetEnd()  )
				{
					throw InvalidDateTimeException( 
						 GetID()
						);
				}
				else
				{
					AllFilmsInRoom.push_back( boost::make_tuple( _Item.m_FilmID, timeBegin, timeEnd ) );
				}
			}
			std::sort( AllFilmsInRoom.begin(), AllFilmsInRoom.end(), CompareFirst() );
			std::vector< FilmTuple_t >::const_iterator it = 
				std::adjacent_find( AllFilmsInRoom.begin(), AllFilmsInRoom.end(), CompareFirstAndSecond() );
			if( it != AllFilmsInRoom.end() )
			{
				int _1stFilmID = it->get<0>();
				++it;
				int _2ndFilmID = it->get<0>();
				throw InvalidDateTimeException( GetID()	);
			}
		}
	}
}

boost::shared_ptr<OutputMessage>	BO_TimeTable::Execute( CCinemaOleDB& db, MyDebugOutputImpl& Debug )
{
	CheckExistance( db, Debug );
	Update( db, Debug );
	CString s = LoadStringInternal( IDS_NOERROR );
	std::wstring ws( s );
	return boost::shared_ptr<OutputMessage>( new CCS_ErrorMessage( GetID(), c_resOk, ws ) );
}

void	BO_TimeTable::CheckExistance( CCinemaOleDB& db, MyDebugOutputImpl& Debug )
{
	FilmMap_t::const_iterator it = m_Films.begin();
	for( ; it != m_Films.end(); ++it )
	{
		int nRoomNo = it->first;
		const std::vector<BO_TimeTable::Item>& ItemArr = it->second;
		std::vector<int> ExchangeID;
		ExchangeID.reserve( 1024 );
		for( size_t i = 0; i < ItemArr.size(); ++i )
		{
			const BO_TimeTable::Item& _Item = ItemArr[i];
			if( _Item.IsNoData() )
			{
				ExchangeID.push_back( _Item.m_FilmID );
			}
		}
		try
		{
			db.GetTableTimetableFacet().CheckExistance( m_CinemaName,  nRoomNo, ExchangeID, GetBegin(), GetEnd() );
		}
		catch(NoSuchCinemaOrRoom)
		{
			throw NoCinemaException( GetID() );
		}
		catch(NoSuchExchangeID)
		{
			throw NoFilmException( GetID() );
		}
	}
}

void	BO_TimeTable::Update( CCinemaOleDB& db, MyDebugOutputImpl& Debug )
{
	FilmMap_t::const_iterator it = m_Films.begin();
	for( ; it != m_Films.end(); ++it )
	{
		int nRoomNo = it->first;
		const std::vector<BO_TimeTable::Item>& ItemArr = it->second;
		TimeTable_t FilmsInRoom;
		std::vector< std::wstring > FilmNames;
		FilmsInRoom.reserve( 1024 );
		std::vector<int> ExchangeID;
		ExchangeID.reserve( 1024 );
		for( size_t i = 0; i < ItemArr.size(); ++i )
		{
			const BO_TimeTable::Item& _Item = ItemArr[i];
			if( !_Item.IsNoData() )
			{
				FilmsInRoom.push_back( FilmTime( _Item.m_FilmID, *_Item.m_Begin.get_ptr(), *_Item.m_End.get_ptr() ) );
				FilmNames.push_back( *_Item.m_Name.get_ptr() );
			}
			else
				ExchangeID.push_back( _Item.m_FilmID );
		}
		try
		{
			FilmTime Period( -1, GetBegin(), GetEnd() );
			db.GetTableTimetableFacet().SetTimetableSmart( m_CinemaName,  nRoomNo, Period, FilmsInRoom, FilmNames, ExchangeID );
		}
		catch(NoSuchCinemaOrRoom)
		{
#if 0
			CStringW s; s.Format( LoadStringInternal(IDS_EE_CINEMA), m_CinemaName.c_str(), nRoomNo );
			throw QueryException( (LPCWSTR) s, GetID() );
#else
			throw NoCinemaException( GetID() );
#endif
		}
		catch(NoSuchExchangeID)
		{
#if 0
			CStringW s; s.Format( LoadStringInternal(IDS_EE_FILMID) );
			throw QueryException( (LPCWSTR)s, GetID() );
#else
			throw NoFilmException( GetID() );
#endif
		}
		catch( InvalidIntervalException& /*ex*/ )
		{
#if 0
			size_t nIndex = ex.GetRecordNumber();
			CStringW s; s.Format( LoadStringInternal(IDS_EE_TIME_BOUNDS) , FilmsInRoom[nIndex].m_nFilmID );
			throw QueryException( (LPCWSTR)s, GetID() );
#else
			throw InvalidDateTimeException( GetID() );
#endif
		}
		catch( IntervalIntersectionException& /*ex*/ )
		{
#if 0
			size_t nIndex = ex.GetRecordNumber();
			CStringW s; s.Format( LoadStringInternal(IDS_EE_INTERSECT), FilmsInRoom[nIndex].m_nFilmID );
			throw QueryException( (LPCWSTR)s, GetID() );
#else
			throw InvalidDateTimeException( GetID() );
#endif
		}
		catch( DataBaseException )
		{
			throw QueryDatabaseException( GetID() );
		}
	}
}