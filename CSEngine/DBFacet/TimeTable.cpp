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
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "TimeTable.h"

/////// accessors /////////

struct DBA_Timetable
{
	int					m_nFilmID;
	int					m_nRoomID;
	LPCWSTR				m_szName;
	DBTIMESTAMP			m_timeBegin;
	DBTIMESTAMP			m_timeEnd;

	size_t					m_nNameLength;

	BEGIN_ACCESSOR_MAP(DBA_Timetable, 2)
		// Accessor 0 is the automatic accessor
		BEGIN_ACCESSOR(0, true)
		COLUMN_ENTRY(1, m_nFilmID )
		END_ACCESSOR()
		// but for insert
		BEGIN_ACCESSOR(1, true)
		COLUMN_ENTRY(2, m_nRoomID)
		COLUMN_ENTRY_LENGTH(3, m_szName, m_nNameLength)
		COLUMN_ENTRY(4, m_timeBegin)
		COLUMN_ENTRY(5, m_timeEnd)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
};


struct DBA_ExchangeTimetable
{
	LPCWSTR				m_szName;
	size_t				m_nNameLength;
	DBTIMESTAMP			m_timeBegin;
	DBTIMESTAMP			m_timeEnd;

	int					m_nExchangeID;
//	int					m_nRoomID;

	BEGIN_ACCESSOR_MAP(DBA_ExchangeTimetable, 2)
		BEGIN_ACCESSOR(0, true)
			COLUMN_ENTRY_LENGTH(1, m_szName, m_nNameLength)
			COLUMN_ENTRY(2, m_timeBegin)
			COLUMN_ENTRY(3, m_timeEnd)
		END_ACCESSOR()
		BEGIN_ACCESSOR(1, false)
			COLUMN_ENTRY_LENGTH(1, m_szName, m_nNameLength)
			COLUMN_ENTRY(2, m_timeBegin)
			COLUMN_ENTRY(3, m_timeEnd)
			COLUMN_ENTRY(4, m_nExchangeID)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
};

struct DBA_ExchangeTimetableInsert
{
	LPCWSTR				m_szName;
	size_t				m_nNameLength;
	DBTIMESTAMP			m_timeBegin;
	DBTIMESTAMP			m_timeEnd;

	int					m_nExchangeID;
	int					m_nRoomID;

	BEGIN_ACCESSOR_MAP(DBA_ExchangeTimetableInsert, 1)
		BEGIN_ACCESSOR(0, false)
		COLUMN_ENTRY_LENGTH(1, m_szName, m_nNameLength)
		COLUMN_ENTRY(2, m_timeBegin)
		COLUMN_ENTRY(3, m_timeEnd)
		COLUMN_ENTRY(4, m_nExchangeID)
		COLUMN_ENTRY(5, m_nRoomID)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
};


struct DBA_ExchangeExistance
{
	int					m_nRoomNo;

	BEGIN_ACCESSOR_MAP(DBA_ExchangeExistance, 1)
		BEGIN_ACCESSOR(0, true)
		COLUMN_ENTRY(1, m_nRoomNo)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
};

//=====================================================================================//
//                                   class TimeTable                                   //
//=====================================================================================//
void TimeTable::ThrowDBError(size_t nIndex)
{
	CDBErrorInfo ErrorInfo;
	ULONG        cRecords;
	HRESULT		 hr;
	hr = ErrorInfo.GetErrorRecords(&cRecords);
	if (FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
	{
		ATLTRACE(atlTraceDBClient, 0, _T("No OLE DB Error Information found: hr = 0x%x\n"), hr);
	}
	else
	{
		for ( ULONG j = 0; j < cRecords; j++)
		{
			ERRORINFO ei;
			hr = ErrorInfo.GetBasicErrorInfo( j, &ei );
			if ( hr == S_OK )
			{
				switch( ei.dwMinor )
				{
				case 50100 : throw InvalidIntervalException( nIndex );
				case 50101 : throw IntervalIntersectionException( nIndex );
				}
			}
		}
	}
}

void	TimeTable::ThrowDBErrorWithAbort( HRESULT hr, size_t i )
{
	if( hr != S_OK )
	try
	{
//		AtlTraceErrorRecords(hr);
		ThrowDBError( i );
	}
	catch( CommonException )
	{
		m_dbSession.Abort();
		throw;
	}
}

void			TimeTable::SetTimetable(int nRoomID, const FilmTime& Range, 
										const TimeTable_t& Timetable, 
										const std::vector<std::wstring>& Names ) 	
{
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	m_dbSession.StartTransaction();

	CCommand<CAccessor<DBA_Timetable> > A_Timetable;
	CString sQuery; HRESULT hr;
	// delete
	CCommand< CNoAccessor > A_DeleteTime;
	CDBPropSet  propsetDel(DBPROPSET_ROWSET);
	propsetDel.AddProperty(DBPROP_IRowsetChange, true);
	propsetDel.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_DELETE );
	CString sBegin = Range.m_timeBegin.Format(" \'%Y-%m-%d %H:%M:%S\' ");
	CString sEnd = Range.m_timeEnd.Format(" \'%Y-%m-%d %H:%M:%S\' ");
	sQuery.Format(	_T(" DELETE FROM Timetable ")
					_T(" WHERE ") 
					_T(" (	(BeginTime >=%s AND BeginTime < %s) OR ")
					_T(		"(EndTime >= %s AND EndTime <%s) ) ") 
					_T(" AND RoomID=%d "),
			(LPCTSTR)sBegin, (LPCTSTR)sEnd,
			(LPCTSTR)sBegin, (LPCTSTR)sEnd, 
			nRoomID);
	hr = A_DeleteTime.Open(m_dbSession, sQuery, &propsetDel );
	if( hr != S_OK  )
	{
		AtlTraceErrorRecords(hr);
		m_dbSession.Abort();
		throw TableException("Timetable delete data error");
	}
	A_DeleteTime.Close();
	/// insert
	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
	propset.AddProperty( DBPROP_SERVERDATAONINSERT, true);

	sQuery.Format( _T("SELECT FilmID, RoomID, Name, BeginTime, EndTime FROM Timetable") );
	hr = A_Timetable.Open(m_dbSession, sQuery, &propset );
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		m_dbSession.Abort();
		throw TableException("Timetable set data error");
	}
	for( size_t i = 0 ; i < Timetable.size(); ++i)
	{
		A_Timetable.m_nRoomID	= nRoomID;
		A_Timetable.m_szName	= Names[i].c_str();
		A_Timetable.m_nNameLength = sizeof(WCHAR) * (Names[i].size());
		Timetable[i].m_timeBegin.GetAsDBTIMESTAMP(A_Timetable.m_timeBegin);
		Timetable[i].m_timeEnd.GetAsDBTIMESTAMP(A_Timetable.m_timeEnd);
		hr = A_Timetable.Insert( 1 );
		if( FAILED(hr) ) 
		{
			try
			{
				ThrowDBError( i );
			}
			catch ( TableException ) 
			{
				m_dbSession.Abort();
				throw;
			}
			A_Timetable.FreeRecordMemory();
			throw TableException("Timetable set data error");
		}
		A_Timetable.FreeRecordMemory();
	}
	m_dbSession.Commit();
}

void	TimeTable::GetTimetable(int nRoomID, const FilmTime& Range, TimeTable_t& Timetable,
									std::vector<std::wstring>& Names) 
{
	Timetable.clear();
	Names.clear();

	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	CCommand<CAccessor<DBA_Timetable> > A_Timetable;
	HRESULT hr;
	CString sBegin = Range.m_timeBegin.Format(" \'%Y-%m-%d %H:%M\' ");
	CString sEnd = Range.m_timeEnd.Format(" \'%Y-%m-%d %H:%M\' ");
	CString sQuery;
	sQuery.Format(	_T(" SELECT FilmID, RoomID, Name, BeginTime, EndTime ")
					_T(" FROM Timetable ")
					_T(" WHERE (BeginTime >= %s AND BeginTime < %s) AND ")
					_T(" (EndTime >= %s AND EndTime < %s) AND RoomID=%d ORDER BY BeginTime"),
		(LPCTSTR)sBegin, (LPCTSTR)sEnd,
		(LPCTSTR)sBegin, (LPCTSTR)sEnd,
		nRoomID);
	hr = A_Timetable.Open(m_dbSession, sQuery );
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		throw TableException("Timetable get data error");
	}
	SYSTEMTIME st;
	while( A_Timetable.MoveNext() == S_OK )
	{
		DBTimeToSystemTime(A_Timetable.m_timeBegin, st);
		CTime timeBegin( st );
		DBTimeToSystemTime(A_Timetable.m_timeEnd, st);
		CTime timeEnd( st);
		Timetable.push_back( FilmTime( A_Timetable.m_nFilmID, timeBegin, timeEnd ) );
		std::wstring s( A_Timetable.m_szName, 
						A_Timetable.m_szName + A_Timetable.m_nNameLength/ sizeof(WCHAR)  );
		Names.push_back( s );
		A_Timetable.FreeRecordMemory();
	}
}

bool		TimeTable::GetCurrentOrNextFilmTime( int nRoomID, FilmTime& Time)
{
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	CTime timeCurrent = CTime::GetCurrentTime();
	CString sNow = timeCurrent.Format(" \'%Y-%m-%d %H:%M\' ");
	CString sQuery;
	sQuery.Format( _T("SELECT FilmID, RoomID, Name, BeginTime, EndTime FROM Timetable WHERE (EndTime > %s) AND RoomID=%d ORDER BY BeginTime ASC"),
		(LPCTSTR)sNow,
		nRoomID);
	std::wstring Name;
	return QueryFilm( std::wstring(sQuery), Time, Name);
}

bool	TimeTable::GetCurrentFilmTime( int nRoomID, FilmTime& Time, std::wstring& Name)
{
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	CTime timeCurrent = CTime::GetCurrentTime();
	CString sNow = timeCurrent.Format(" \'%Y-%m-%d %H:%M\' ");
	CString sQuery;
	sQuery.Format( _T("SELECT FilmID, RoomID, Name, BeginTime, EndTime FROM Timetable WHERE ( %s >= BeginTime AND %s < EndTime) AND RoomID=%d"),
		(LPCTSTR)sNow, (LPCTSTR)sNow, nRoomID);
	return QueryFilm( std::wstring(sQuery), Time, Name );
}

bool	TimeTable::GetFilmFromId( int nFilmID, FilmTime& Time, std::wstring& Name  )
{
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	CString sQuery;
	sQuery.Format( _T("SELECT FilmID, RoomID, Name, BeginTime, EndTime FROM Timetable WHERE FilmID=%d"),
					nFilmID);
	return QueryFilm( std::wstring(sQuery), Time, Name );
}

bool	TimeTable::GetPreviousCinemaFilm( int nRoomID, CTime& timeEnd )
{
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Time table connection error");
	}

	CTime timeCurrent = CTime::GetCurrentTime();
	CString sNow = timeCurrent.Format(" \'%Y-%m-%d %H:%M\' ");

	FilmTime Time;
	std::wstring sName;
	CString sQuery;
	sQuery.Format( 
		_T("select top 1 FilmID, RoomID, Name, BeginTime, EndTime  from TimeTable ")
		_T("where RoomID in (select B.RoomID from Rooms as A, Rooms as B ") 
						 _T("where A.CinemaId = B.CinemaId and A.RoomID = %d) ")
					 _T("and EndTime < %s ")
		_T("order by EndTime desc "),
		nRoomID,
		sNow);
	bool res = QueryFilm( std::wstring(sQuery), Time, sName );
	timeEnd = Time.m_timeEnd;
	return res;
}

bool	TimeTable::QueryFilm( const std::wstring& sQuery,  FilmTime& Time, std::wstring& Name )
{
	HRESULT hr;
	CCommand<CAccessor<DBA_Timetable> > A_Timetable;
	hr = A_Timetable.Open(m_dbSession, sQuery.c_str() );
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		throw TableException("Timetable get data error");
	}
	SYSTEMTIME st;
	if( A_Timetable.MoveNext() == S_OK )
	{
		DBTimeToSystemTime(A_Timetable.m_timeBegin, st);
		CTime timeBegin( st );
		DBTimeToSystemTime(A_Timetable.m_timeEnd, st);
		CTime timeEnd( st);
		Name = A_Timetable.m_nNameLength ? A_Timetable.m_szName : L"";
		Time = FilmTime( A_Timetable.m_nFilmID, timeBegin, timeEnd ) ;
		A_Timetable.FreeRecordMemory();
		return true;
	}
	else return false;
}

////////////////////////////////// new functions

static void CheckResult(HRESULT hr)
{
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		throw TableException("Timetable get data error");
	}
}

void	TimeTable::CheckExistance(	const std::wstring& sCinemaName, int nRoomNo, 
									const std::vector<int>& ExchangeID, 
									CTime timeBegin, CTime timeEnd ) // throw NoSuchExchangeID
{
	HRESULT hr;
	CheckConnection();
	CCommand< CAccessor< DBA_ExchangeExistance> >	A_Timetable;

	CStringW sBegin = timeBegin.Format( L"%Y-%m-%d %H:%M" );
	CStringW sEnd = timeEnd.Format( L"%Y-%m-%d %H:%M" );
	CStringW sFmtQuery, sFullQuery;
	sFullQuery.Format(	L"SELECT RoomNumber FROM TimetableView " 
						L"WHERE CinemaName='%s' AND RoomNumber=%d", sCinemaName.c_str(), nRoomNo );
	hr = A_Timetable.Open( m_dbSession, sFullQuery );
	CheckResult(hr);
	if( A_Timetable.MoveNext() != S_OK ) throw NoSuchCinemaOrRoom("");
	A_Timetable.Close();
	sFmtQuery.Format(	L"SELECT RoomNumber FROM TimetableView " 
						L"WHERE CinemaName='%s' AND RoomNumber=%d AND ExchangeID=%%d AND "
						L"((BeginTime >= '%s' AND BeginTime < '%s' ) OR ( EndTime >= '%s' AND EndTime < '%s'))",
						sCinemaName.c_str(), nRoomNo,
						(LPCWSTR)sBegin, (LPCWSTR)sEnd, (LPCWSTR)sBegin, (LPCWSTR)sEnd
						);
	for( size_t i = 0; i < ExchangeID.size(); ++i )
	{		
		sFullQuery.Format( sFmtQuery, ExchangeID[i] );
		hr = A_Timetable.Open(m_dbSession, sFullQuery );
		CheckResult(hr);
		if( A_Timetable.MoveNext() != S_OK ) throw NoSuchExchangeID("");
		A_Timetable.Close();
	}
}

void	TimeTable::SetTimetableSmart(	const std::wstring& sCinemaName,
										int nRoomNo,
										const FilmTime& Range,
										const TimeTable_t& Timetable, 
										const std::vector<std::wstring>& Names,
										const std::vector<int>& ExchangeID )  // throws IntervalIntersectionException
{
	CheckConnection();
	HRESULT hr;

	std::vector<int>	AllValidIDs ( ExchangeID );
	AllValidIDs.reserve( 1024 );

	for( size_t i = 0; i < Timetable.size(); ++i )
	{		
		if( Timetable[i].m_nFilmID != FilmTime::s_nUserFilmID )
			AllValidIDs.push_back( Timetable[i].m_nFilmID );
	}
	CStringW sBegin = Range.m_timeBegin.Format(L"%Y-%m-%d %H:%M");
	CStringW sEnd = Range.m_timeEnd.Format(L"%Y-%m-%d %H:%M");
	CStringW sDeleteQuery, sDeleteQueryAdd;
	if( ! AllValidIDs.empty() )
	{
		CStringW sTemp;
		sDeleteQueryAdd.Format( L" AND ExchangeID NOT IN (%d", AllValidIDs[0] );
		for( size_t i =1; i < AllValidIDs.size(); ++i )
		{
			sTemp.Format( L", %d", AllValidIDs[i] );
			sDeleteQueryAdd += sTemp;
		}
		sDeleteQueryAdd += L")";	
	}
	sDeleteQuery.Format
		(
		L"DELETE  FROM Timetable "
		L"WHERE FilmID IN "
		L"(" 
		L"SELECT FilmID FROM TimetableView "
		L"WHERE "
		L"TimetableView.CinemaName='%s' AND TimetableView.RoomNumber=%d AND "
		L"((TimetableView.BeginTime >= '%s' AND TimetableView.BeginTime < '%s' ) OR "
		L"( TimetableView.EndTime >= '%s' AND TimetableView.EndTime < '%s')) %s "
		L")",
		sCinemaName.c_str(), nRoomNo,
		(LPCWSTR)sBegin, (LPCWSTR)sEnd, (LPCWSTR)sBegin, (LPCWSTR)sEnd,
		(LPCWSTR)sDeleteQueryAdd
		);

	CDBPropSet  propsetDel(DBPROPSET_ROWSET);
	propsetDel.AddProperty(DBPROP_IRowsetChange, true);
	propsetDel.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_DELETE );

	CCommand< CNoAccessor >	A_Delete;
	m_dbSession.StartTransaction();
	hr = A_Delete.Open(m_dbSession, sDeleteQuery, &propsetDel);
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		m_dbSession.Abort();
		throw TableException("Timetable get data error");
	}

	CCommand< CAccessor< DBA_ExchangeTimetable> >	A_Timetable;
	for( size_t i = 0; i < Timetable.size(); ++i )
	{		
		CStringW sQuery;
		sQuery.Format(	
			L"SELECT Name, BeginTime, EndTime, ExchangeID "
			L"FROM TimetableView "
			L"WHERE FilmID IN "
			L"( SELECT FilmID FROM TimetableView WHERE "
			L"TimetableView.CinemaName='%s' AND TimetableView.RoomNumber=%d AND "
			L"((TimetableView.BeginTime >= '%s' AND TimetableView.BeginTime < '%s' ) OR "
			L"( TimetableView.EndTime >= '%s' AND TimetableView.EndTime < '%s')) AND "
			L"TimetableView.ExchangeID=%d "
			L")",
			sCinemaName.c_str(), nRoomNo,
			(LPCWSTR)sBegin, (LPCWSTR)sEnd, (LPCWSTR)sBegin, (LPCWSTR)sEnd, Timetable[i].m_nFilmID
			);

		CDBPropSet  propsetUpd(DBPROPSET_ROWSET);
		propsetUpd.AddProperty(DBPROP_IRowsetChange, true);
		propsetUpd.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE|DBPROPVAL_UP_INSERT );
		hr = A_Timetable.Open(m_dbSession, sQuery, &propsetUpd );
		ThrowDBErrorWithAbort(hr, i);
		if( A_Timetable.MoveNext() == S_OK )
		{
			A_Timetable.m_szName		= Names[i].c_str();
			A_Timetable.m_nNameLength	= sizeof(WCHAR) * Names[i].length();
			Timetable[i].m_timeBegin.GetAsDBTIMESTAMP( A_Timetable.m_timeBegin );
			Timetable[i].m_timeEnd.GetAsDBTIMESTAMP( A_Timetable.m_timeEnd );
			hr = A_Timetable.SetData(0);
		}
		else
		{			
			CCommand< CAccessor< DBA_ExchangeExistance> >	A_ExchangeExistance;
			sQuery.Format
				(	
				L"SELECT RoomID FROM Rooms JOIN Cinemas ON Rooms.CinemaID=Rooms.CinemaID "
				L"WHERE Cinemas.CinemaName='%s' AND Rooms.RoomNumber=%d ",
				sCinemaName.c_str(), nRoomNo
				);
			hr = A_ExchangeExistance.Open( m_dbSession, sQuery );
			ThrowDBErrorWithAbort(hr, i);
			hr = A_ExchangeExistance.MoveNext();
			if( hr != S_OK ) throw NoSuchCinemaOrRoom("");
			int nRoomID = A_ExchangeExistance.m_nRoomNo;
			A_ExchangeExistance.Close();
			sQuery.Format
				(	
				L"SELECT Name, BeginTime, EndTime, ExchangeID, RoomID "
				L"FROM Timetable "
				);
			CDBPropSet  propsetUpd(DBPROPSET_ROWSET);
			propsetUpd.AddProperty(DBPROP_IRowsetChange, true);
			propsetUpd.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
//			propsetUpd.AddProperty( DBPROP_SERVERDATAONINSERT, true);
			CCommand< CAccessor<DBA_ExchangeTimetableInsert> > A_TimetableInsert;
			hr = A_TimetableInsert.Open(m_dbSession, sQuery, &propsetUpd );
			if( hr != S_OK ) AtlTraceErrorRecords( hr );
			A_TimetableInsert.m_szName			= Names[i].c_str();
			A_TimetableInsert.m_nNameLength		= sizeof(WCHAR) * Names[i].length();
			A_TimetableInsert.m_nExchangeID		= Timetable[i].m_nFilmID;
			A_TimetableInsert.m_nRoomID			= nRoomID;
			Timetable[i].m_timeBegin.GetAsDBTIMESTAMP( A_TimetableInsert.m_timeBegin );
			Timetable[i].m_timeEnd.GetAsDBTIMESTAMP( A_TimetableInsert.m_timeEnd );
			hr = A_TimetableInsert.Insert( 0 );
			AtlTraceErrorRecords( hr );
		}
		ThrowDBErrorWithAbort( hr, i );
		A_Timetable.Close();
	}
	m_dbSession.Commit();
}