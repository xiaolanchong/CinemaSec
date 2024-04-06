#pragma once

#include "../../CSChair/Common/Exception.h"
#include "../../CSChair/Common/tstring.h"
#include <atltime.h>

//typedef std::pair<CTime, CTime>			FilmTime;
struct FilmTime
{
	int		m_nFilmID;
	CTime	m_timeBegin;
	CTime	m_timeEnd;
//	int		m_nExchangeID;

	static const int	s_nUserFilmID = -1;

	FilmTime( int id, CTime b, CTime e ) :
		m_nFilmID(id), m_timeBegin(b), m_timeEnd(e)
	{

	}
	FilmTime (  ) : m_nFilmID(s_nUserFilmID){}
};
typedef std::vector< FilmTime  >		TimeTable_t;


//MACRO_EXCEPTION( TimeHelperException, CommonException );

class TimeHelperException : public CommonException
{
	size_t nErrorIndex;
public:
	size_t GetErrorIndex() const { return nErrorIndex;}
	TimeHelperException(size_t s) : nErrorIndex(s), CommonException("TimeHelper exception")
	{
	}
};

class TimeHelperLateBeginException : public TimeHelperException
{
public:
	TimeHelperLateBeginException(size_t s) : TimeHelperException(s) {}
};

class TimeHelperIntersectionException : public TimeHelperException
{
public:
	TimeHelperIntersectionException(size_t s) : TimeHelperException(s) {}
};

class TimeHelper
{
	struct SelectFirst
	{
		bool operator ()(const FilmTime& t1, const FilmTime& t2)
		{
			return t1.m_timeBegin < t2.m_timeBegin;
		}
	};
	struct SelectSecond
	{
		bool operator ()(const FilmTime& t1, const FilmTime& t2)
		{
			// NOTE if u change it? change int db trigger too !!!
			return t1.m_timeEnd >= t2.m_timeBegin;
		}
	};
	struct FindPair
	{
		bool operator ()(const FilmTime& t1)
		{
			return t1.m_timeBegin >= t1.m_timeEnd;
		}
	};
public:

	bool	Validate( const TimeTable_t& TimeTable);
};

inline bool	TimeHelper::Validate( const TimeTable_t& TimeTable)
{
	std::vector<FilmTime> TimeArr(TimeTable);

	std::sort( TimeArr.begin(), TimeArr.end(), TimeHelper::SelectFirst() );
	TimeTable_t::const_iterator it;
	it = std::find_if( TimeArr.begin(), TimeArr.end(), FindPair() );
	if( it !=  TimeArr.end() )
	{
		throw TimeHelperLateBeginException( it - TimeArr.begin() );
//		return false;
	}
	it = std::adjacent_find( TimeArr.begin(), TimeArr.end(), SelectSecond() );
	if( it !=  TimeArr.end() )
	{
		throw TimeHelperIntersectionException( it - TimeArr.begin() );
//		return false;
	}
	return true;
}