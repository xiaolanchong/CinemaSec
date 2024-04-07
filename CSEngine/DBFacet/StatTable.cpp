//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		OLE DB database helper class for 'Statisitcs' & 'StatPhoto' tables access
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   12.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "StatTable.h"


/////// Table accessors ///////////////

struct DBA_Statistics
{
	int				m_nStatID;
	int				m_nRoomID;
	DBTIMESTAMP		m_Time;
	int				m_nSpectators;

	BEGIN_ACCESSOR_MAP(DBA_Statistics, 2)
		// Accessor 0 is the automatic accessor
		BEGIN_ACCESSOR(0, true)
			COLUMN_ENTRY(1, m_nStatID)
		END_ACCESSOR()
		// Accessor 1 is the automatic accessor, but for insert
		BEGIN_ACCESSOR(1, true)
			COLUMN_ENTRY(2, m_nRoomID)
			COLUMN_ENTRY(3, m_Time)
			COLUMN_ENTRY(4, m_nSpectators)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Statistics, _T("SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics]"))
};

struct DBA_StatPhotos
{
	int					m_nStatID;
	int					m_nCameraID;

	ISequentialStream*	m_BLOBDATA;
	ULONG				m_BLOBDATA_LENGTH;
	ULONG				m_BLOBDATA_STATUS;

	BEGIN_COLUMN_MAP(DBA_StatPhotos)
		COLUMN_ENTRY(1, m_nStatID)
		COLUMN_ENTRY(2, m_nCameraID)
		BLOB_ENTRY_LENGTH_STATUS(3, IID_ISequentialStream, STGM_READ, m_BLOBDATA, m_BLOBDATA_LENGTH, m_BLOBDATA_STATUS)
	END_COLUMN_MAP()
	DEFINE_COMMAND_EX(DBA_StatPhotos, _T("SELECT StatID, CameraID, StatPhoto FROM StatPhotos"))
};

struct DBA_GetStatID
{
	int					m_nStatID;

	BEGIN_PARAM_MAP(DBA_GetStatID)
		SET_PARAM_TYPE(DBPARAMIO_OUTPUT)
		COLUMN_ENTRY(1, m_nStatID)   // name corresponds to first '?' param
	END_PARAM_MAP()
	DEFINE_COMMAND_EX(DBA_GetStatID, _T("{ ? = call GetNextStatID }"))
};

struct DBA_ExchangeStat
{
	int					m_nExchangeID;
	int					m_nSpectators;

	DWORD				m_dwSpectatorStatus;

	BEGIN_COLUMN_MAP(DBA_ExchangeStat)
		COLUMN_ENTRY(1, m_nExchangeID)
		COLUMN_ENTRY_STATUS(2, m_nSpectators, m_dwSpectatorStatus)
	END_COLUMN_MAP()
};

//=====================================================================================//
//                                   class StatTable                                   //
//=====================================================================================//

void	StatTable::SetStatisticsRow(int nRoomID, const DBTIMESTAMP& Time, int nSpectators, 
									   const std::map< int, std::vector<BYTE> >& BigImage )
{
	CCommand<CAccessor<DBA_Statistics> > A_Statistics;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}

	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
	propset.AddProperty( DBPROP_SERVERDATAONINSERT, true);

	std::wstring sQuery(  L"SELECT StatID, RoomID, StatTime, StatViewerNumber  FROM [Statistics]" );
	hr = A_Statistics.Open(m_dbSession, sQuery.c_str(), &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}
	// Get data from the rowset

	int nStatID;
	A_Statistics.m_nRoomID			= nRoomID;
	A_Statistics.m_nSpectators		= nSpectators;
	A_Statistics.m_Time				= Time;
	hr = A_Statistics.Insert(1, true);	
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		A_Statistics.FreeRecordMemory();
		throw TableException("Statistics set data error");
	}
	hr = A_Statistics.GetData( 0 );
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		A_Statistics.FreeRecordMemory();
		throw TableException("Statistics set data error");
	}
	nStatID = A_Statistics.m_nStatID;
	A_Statistics.FreeRecordMemory();

	A_Statistics.Close();

	//for( size_t i = 0; i < BigImage
	std::map<int , std::vector<BYTE> > ::const_iterator it = 
		BigImage.begin();

	CCommand<CAccessor<DBA_StatPhotos> > A_StatPhotos;

	CDBPropSet  propset2(DBPROPSET_ROWSET);
	propset2.AddProperty(DBPROP_IRowsetChange, true);
	propset2.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
	hr = A_StatPhotos.Open(m_dbSession, 0, &propset2);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}
	for(; it != BigImage.end(); ++it)
	{
		int nCameraID = it->first;
		ISSHelper::MemStreamRead msr( it->second );

		A_StatPhotos.m_nStatID		= nStatID;
		A_StatPhotos.m_nCameraID	= nCameraID;
		A_StatPhotos.m_BLOBDATA_STATUS	= DBSTATUS_S_OK;
		A_StatPhotos.m_BLOBDATA			= &msr;
		A_StatPhotos.m_BLOBDATA_LENGTH	= ULONG(it->second.size());
		hr = A_StatPhotos.Insert( );
		if( FAILED(hr) ) 
		{
			AtlTraceErrorRecords(hr);
			A_Statistics.FreeRecordMemory();
			throw TableException("Statistics set data error");
		}
		A_Statistics.FreeRecordMemory();
	}
	A_StatPhotos.Close();
}
#ifdef QUERY_ALL_STATISTICS
const StatRowSet_t&			StatTable::GetStat() const	
{
	return m_StatSet;
}

void						StatTable::SelectFromLastQuery()	
{
	CCommand<CAccessor<DBA_Statistics> > A_Statistics;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}
	HRESULT hr;
	std::wstring sQuery;
	if( m_LastQueryTime.first )
	{
		const DBTIMESTAMP& dbTimeStamp = m_LastQueryTime.second;
		COleDateTime d(	dbTimeStamp.year, dbTimeStamp.month, dbTimeStamp.day,  
			dbTimeStamp.hour, dbTimeStamp.minute, dbTimeStamp.second);

		CString sTime = d.Format( _T("%Y-%m-%d %H:%M:%S") );
		//if(var.vt != VT_BSTR ) throw TableException("Invalid time");
		sQuery = L"SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics] WHERE StatTime > ";
		sQuery += L"'";
		sQuery += (LPCWSTR)sTime;
		sQuery += L"'";
	}
	else
	{
		sQuery = L"SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics]";
	}
	hr = A_Statistics.Open(m_dbSession, sQuery.c_str());
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	while( A_Statistics.MoveNext() == S_OK )
	{
		m_StatSet.insert( StatRow( A_Statistics.m_nStatID, A_Statistics.m_nRoomID, 
			A_Statistics.m_Time, A_Statistics.m_nSpectators ) );
	}

	SYSTEMTIME st;
	GetLocalTime(&st);
	DBTIMESTAMP Time;
	SystemTimeToDBTime(st, Time);
	m_LastQueryTime = std::make_pair( true, Time );
}
#endif

PhotoMap_t	StatTable::GetImages(int nStatID)	
{
	CCommand<CAccessor<DBA_StatPhotos> > A_StatPhotos;
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}
	CString sQuery; HRESULT hr;
	sQuery.Format( _T("SELECT StatID, CameraID, StatPhoto FROM StatPhotos WHERE StatID=%d"), nStatID );
	hr = A_StatPhotos.Open(m_dbSession, sQuery , 0);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}
	std::vector<BYTE> Arr;
	PhotoMap_t	PhMap;
	TCHAR szBuf[1024];
	while( A_StatPhotos.MoveNext() == S_OK )
	{
		Arr.clear();
		int nCameraID = A_StatPhotos.m_nCameraID;
		PhotoCacheMap_t::const_iterator it = 
			m_PhotoCache.find( std::make_pair( nStatID, nCameraID ) );
		if(it == m_PhotoCache.end() &&
			A_StatPhotos.m_BLOBDATA_STATUS == DBSTATUS_S_OK )
		{
			ISSHelper::ReadBlobToArray( Arr, A_StatPhotos.m_BLOBDATA, A_StatPhotos.m_BLOBDATA_LENGTH);
			PhotoCacheMap_t::iterator itIns = m_PhotoCache.insert( std::make_pair( std::make_pair( nStatID, nCameraID ), Arr ) ).first;

			StringCchPrintf( szBuf, 1024, _T("LoadImage StatId=%d CamId=%d\n"), nStatID, nCameraID );
			PhMap.insert( std::make_pair( nCameraID, &itIns->second ) );
		}
		else
		{
			StringCchPrintf( szBuf, 1024, _T("CacheImage StatId=%d CamId=%d\n"), nStatID, nCameraID );
			PhMap.insert( std::make_pair( nCameraID, &it->second ) );
		}
		OutputDebugString( szBuf );
		A_StatPhotos.FreeRecordMemory();
	}
	A_StatPhotos.Close();
	return PhMap;
}

void	StatTable::GetStatForPeriod( CTime timeBegin, CTime timeEnd, int nRoomID, StatRowSet_t& Stats )
{
	HRESULT hr;
	CCommand<CAccessor<DBA_Statistics> > A_Statistics;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}
	CString sBegin	= timeBegin.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sEnd	= timeEnd.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sQuery ;
	sQuery.Format ( L"SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics] WHERE (StatTime BETWEEN %s AND %s) AND RoomID=%d",
					(LPCWSTR)sBegin, (LPCWSTR)sEnd, nRoomID );
	hr = A_Statistics.Open(m_dbSession, sQuery);
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	while( A_Statistics.MoveNext() == S_OK )
	{
		Stats.insert( StatRow( A_Statistics.m_nStatID, A_Statistics.m_nRoomID, 
			ToCTime(A_Statistics.m_Time), A_Statistics.m_nSpectators ) );
	}
}

void	StatTable::GetStatForPeriodReport(	CTime timeBegin, CTime timeEnd, 
										   const std::vector<int>& Rooms, StatRowSet_t& Stats )
{
	HRESULT hr;
	CCommand<CAccessor<DBA_Statistics> > A_Statistics;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}	
	CString sBegin	= timeBegin.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sEnd	= timeEnd.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sWhere;
	ASSERT( Rooms.size() );
	for( size_t i = 0; i < Rooms.size(); ++i )
	{
		CString z;
		z.Format( _T("%d"), Rooms[i]);
		sWhere += z + ( (i != (Rooms.size() - 1))? CString(_T(",")) : CString(_T("")) );
	}

	CString sQuery ;
	sQuery.Format ( L"SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics] WHERE (StatTime BETWEEN %s AND %s) AND RoomID IN (%s) ORDER BY StatTime, RoomID",
		(LPCWSTR)sBegin, (LPCWSTR)sEnd, (LPCWSTR)sWhere );
	hr = A_Statistics.Open(m_dbSession, sQuery);
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	while( A_Statistics.MoveNext() == S_OK )
	{
		Stats.insert( StatRow( A_Statistics.m_nStatID, A_Statistics.m_nRoomID, 
			ToCTime(A_Statistics.m_Time), A_Statistics.m_nSpectators ) );
	}
}

void	StatTable::GetStatInfo( int nStatID, int& nRoomID, CTime& timeStat, int& nSpectators )
{
	HRESULT hr;
	CCommand<CAccessor<DBA_Statistics> > A_Statistics;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}	

	CString sQuery ;
	sQuery.Format ( L"SELECT StatID, RoomID, StatTime, StatViewerNumber FROM [Statistics] WHERE StatID=%d", nStatID );
	hr = A_Statistics.Open(m_dbSession, sQuery);
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	if( A_Statistics.MoveNext() == S_OK )
	{
		nRoomID		= A_Statistics.m_nRoomID;
		timeStat	= ToCTime(A_Statistics.m_Time); 
		nSpectators	= A_Statistics.m_nSpectators;
	}
//	return true;
}

void	StatTable::GetStatForExchange( int /*nRoomID*/, CTime timeBegin, CTime timeEnd, std::vector< FilmStat_t >& StatArr,int &nStatID )
{
	StatArr.clear();
	HRESULT hr;
	if( !m_dbSession.m_spOpenRowset )
	{
		throw TableException("Statistics table connection error");
	}
	if( !CheckConnection() )
	{
		throw DBMustReconnectException ("Reconnection required" ); 
	}

	CCommand<CAccessor<DBA_GetStatID> > A_StatID;
	hr = A_StatID.Open( m_dbSession );
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	nStatID = A_StatID.m_nStatID;

	////////////
	CCommand<CAccessor<DBA_ExchangeStat> > A_Statistics;

	CString sBegin	= timeBegin.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sEnd	= timeEnd.Format( _T("'%Y-%m-%d %H:%M:%S'") );

	CStringW sQuery;
	sQuery.Format( 
	L"select "
		L"[TimeTable].ExchangeID, "
		L"max([Statistics].StatViewerNumber ) "
		L"from [TimeTable] left join [Statistics] "
		L"ON " 
			L"[TimeTable].RoomID = [Statistics].RoomID		AND "
			L"[TimeTable].BeginTime 	< 	[Statistics].StatTime 	AND "
			L"[TimeTable].EndTime 	>	[Statistics].StatTime  "	
			L" "
			L"where	[TimeTable].BeginTime 	>	%s	AND "
			L"[TimeTable].BeginTime 	<	%s	AND "
			L"[TimeTable].EndTime 	>	%s	AND "
			L"[TimeTable].EndTime 	<	%s "	
			L"group by [TimeTable].RoomID, [TimeTable].FilmID, [TimeTable].ExchangeID, [TimeTable].BeginTime " 
			L"order by [TimeTable].RoomID, [TimeTable].BeginTime  ",
			(LPCWSTR)sBegin, (LPCWSTR)sEnd,
			(LPCWSTR)sBegin, (LPCWSTR)sEnd
			);
	hr = A_Statistics.Open(m_dbSession, sQuery);
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Statistics table connection error");
	}

	const int c_WrongSpec = -1;
	while( A_Statistics.MoveNext() == S_OK )
	{
		int nSpecs = A_Statistics.m_dwSpectatorStatus == DBSTATUS_S_ISNULL ? 
						c_WrongSpec : A_Statistics.m_nSpectators;
		StatArr.push_back( FilmStat_t( A_Statistics.m_nExchangeID, nSpecs ) );
	}
}