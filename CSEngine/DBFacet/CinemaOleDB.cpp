#include "stdafx.h"
#include ".\cinemaoledb.h"

// CinemaDB.cpp: implementation of the CCinemaOleDB class.
//
//////////////////////////////////////////////////////////////////////
#include "../stdafx.h"
#include "CinemaDB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct DBA_Blob
{
	ISequentialStream*	m_BLOBDATA;

	ULONG m_BLOBDATA_LENGTH;
	ULONG m_BLOBDATA_STATUS;

	BEGIN_COLUMN_MAP(DBA_Blob)
		BLOB_ENTRY_LENGTH_STATUS(1, IID_ISequentialStream, STGM_READ, m_BLOBDATA, m_BLOBDATA_LENGTH, m_BLOBDATA_STATUS)
	END_COLUMN_MAP()
};

struct DBA_Cinema
{
	int			m_nCinemaID;
	TCHAR		m_szName[80];
	TCHAR		m_szComment[80];

	// output binding map
	BEGIN_COLUMN_MAP(DBA_Cinema)
		COLUMN_ENTRY(1, m_nCinemaID)
		COLUMN_ENTRY(2, m_szName)
		COLUMN_ENTRY(3, m_szComment)
	END_COLUMN_MAP()
	DEFINE_COMMAND_EX(DBA_Cinema, _T("SELECT CinemaID, CinemaName, CinemaComments FROM Cinemas" ))
};

struct DBA_Room
{
	int			m_nRoomID;
	int			m_nCinemaID;
	int			m_nNumber;
	TCHAR		m_szIP[256];

	// output binding map
	BEGIN_ACCESSOR_MAP( DBA_Room, 1)
		BEGIN_ACCESSOR( 0, true )
			COLUMN_ENTRY(1, m_nRoomID)
			COLUMN_ENTRY(2, m_nCinemaID)
			COLUMN_ENTRY(3, m_nNumber)
			COLUMN_ENTRY(4, m_szIP)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Room, _T("SELECT RoomID, CinemaID, RoomNumber, RoomIP FROM Rooms"))
};

struct DBA_Room_Chairs : DBA_Blob
{
};

struct DBA_Room_Time
{
//	int		m_nRoomID;
	int		m_nTimeAfter;
	int		m_nTimeBefore;
	int		m_nTimePeriod;

	BEGIN_COLUMN_MAP(DBA_Room_Time)
//		COLUMN_ENTRY( 1, m_nRoomID )
		COLUMN_ENTRY( 1, m_nTimeAfter )
		COLUMN_ENTRY( 2, m_nTimeBefore )
		COLUMN_ENTRY( 3, m_nTimePeriod )
	END_COLUMN_MAP()
//	DEFINE_COMMAND_EX(DBA_Room, _T("SELECT TimeAfter, TimeBefore, TimePeriod FROM Rooms"))
};

struct DBA_Room_Backup
{
	//	int		m_nRoomID;
	double		m_fStartPercent;
	double		m_fEndPercent;
	int			m_nDaysKeep;

	BEGIN_COLUMN_MAP(DBA_Room_Backup)
		//		COLUMN_ENTRY( 1, m_nRoomID )
		COLUMN_ENTRY( 1, m_fStartPercent )
		COLUMN_ENTRY( 2, m_fEndPercent )
		COLUMN_ENTRY( 3, m_nDaysKeep )
	END_COLUMN_MAP()
};

struct DBA_Camera
{
	int			m_nCameraID;
	int			m_nRoomID;
	int			m_Type;
	DWORD		m_Coord;
	TCHAR		m_szUUID[256];
	TCHAR		m_szIP[256];

	// output binding map
	BEGIN_ACCESSOR_MAP( DBA_Camera, 2 )
		BEGIN_ACCESSOR(0, true)
			COLUMN_ENTRY(1, m_nCameraID)
			COLUMN_ENTRY(2, m_nRoomID)
			COLUMN_ENTRY(3, m_Type)
			COLUMN_ENTRY(4, m_Coord)
			COLUMN_ENTRY(5, m_szUUID)
			COLUMN_ENTRY(6, m_szIP)
		END_ACCESSOR()
		BEGIN_ACCESSOR(1, false)
			COLUMN_ENTRY(4, m_Coord)
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Camera, _T("	SELECT CameraID, RoomID, CameraType, CameraCoord, CameraUID, CameraIP	\
									 FROM Cameras"
									 ))
};


struct DBA_Camera_Background : DBA_Blob
{
};

struct DBA_Room_Model : DBA_Blob
{
};

inline std::pair<int, int > CoordUnpack( int c )
{
	std::pair<int, int> p;
	p.first		= WORD((c) & 0xFFFF);
	p.second	= WORD((c >> 16) & 0xFFFF);
	return p;
}

inline int CoordPack( const std::pair<int, int >& c) 
{
	return (c.first & 0xFFFF) | ( (c.second & 0xFFFF) << 16 );
}

///////////// constructor //////////

CCinemaOleDB::CCinemaOleDB() : m_bForceFailed(true)
{

}

CCinemaOleDB::~CCinemaOleDB()
{

}

void	CCinemaOleDB::Open()
{
	m_bForceFailed = true;
//	bool		nConnect = GetCfgDB().GetDBMode() == CfgDB::dbm_mssql;
//	if( !nConnect ) return;
	std::tstring  strCS = m_sSource;
	try
	{
		COleDBConnection::Open(strCS, m_bShowWindow);
		std::tstring  strCS = COleDBConnection::GetConnectionString();
//		GetCfgDB().SetConnectionString(strCS);
	}
	catch(DBConnectionException&)
	{
		throw;
	}
}

void	CCinemaOleDB::ReadCameraTable()
{
	CCommand<CAccessor<DBA_Camera> > A_Camera;
	HRESULT hr;
	hr = A_Camera.Open(m_dbSession);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Camera table connection error");
	}
	// Get data from the rowset
	while (A_Camera.MoveNext() == S_OK)
	{
		std::pair<int, int> p =  CoordUnpack( A_Camera.m_Coord );
		m_Cameras.insert(  CameraRow(	A_Camera.m_nCameraID, A_Camera.m_nRoomID, 
										p, A_Camera.m_szUUID,
										A_Camera.m_szIP) );
	}	
}

void	CCinemaOleDB::ReadCinemaTable()
{
	CCommand<CAccessor<DBA_Cinema> > A_Cinema;
	HRESULT hr;
	hr = A_Cinema.Open(m_dbSession);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Cinema table connection error");
	}
	// Get data from the rowset
	while (A_Cinema.MoveNext() == S_OK)
	{
		m_Cinemas.insert(  CinemaRow( A_Cinema.m_nCinemaID, A_Cinema.m_szName,  A_Cinema.m_szComment ) );
	}	
}

void	CCinemaOleDB::ReadRoomTable()
{
	CCommand<CAccessor<DBA_Room> > A_Room;
	HRESULT hr;
	hr = A_Room.Open(m_dbSession);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Room table connection error");
	}
	// Get data from the rowset
	while (A_Room.MoveNext() == S_OK)
	{
		m_Rooms.insert(  RoomRow( A_Room.m_nRoomID, A_Room.m_nCinemaID,  A_Room.m_nNumber, A_Room.m_szIP ) );
	}	
}

void		CCinemaOleDB::ReadData()
{
//	bool nConnect = (GetCfgDB().GetDBMode() == CfgDB::dbm_mssql);
//	if( !nConnect ) return;
	if( !m_dbSession.m_spOpenRowset) throw TableException("Table connection error");
	ReadCinemaTable();
	ReadRoomTable();
	ReadCameraTable();
	m_bForceFailed = false;
}

bool			CCinemaOleDB::GetBackground(int nCameraID, std::vector<BYTE>& Data)	
{
	if( !m_dbSession.m_spOpenRowset ) return false;

	CString strQuery;
	strQuery.Format(_T("SELECT CameraBackground FROM	Cameras WHERE	CameraID = %d"), nCameraID);
	return ReadBlobData<DBA_Camera_Background>( m_dbSession, strQuery, Data );
}

bool			CCinemaOleDB::SetBackground(int nCameraID, const std::vector<BYTE>& Data)	
{
	if( !m_dbSession.m_spOpenRowset ) return false;

	CString strQuery;
	strQuery.Format(_T("SELECT CameraBackground FROM	Cameras WHERE	CameraID = %d"), nCameraID);
	return WriteBlobData<DBA_Camera_Background>( m_dbSession,strQuery, Data );
}

bool			CCinemaOleDB::GetChairs(int nRoomID, std::vector<BYTE>& Data)			
{
	if( !m_dbSession.m_spOpenRowset ) return false;
	CString strQuery;
	strQuery.Format(_T("SELECT RoomChairs FROM	Rooms WHERE	RoomID = %d"), nRoomID);
	return ReadBlobData<DBA_Room_Chairs>( m_dbSession,strQuery, Data );
}

bool			CCinemaOleDB::SetChairs(int nRoomID, const std::vector<BYTE>& Data)				
{
	if( !m_dbSession.m_spOpenRowset ) return false;
	CString strQuery;
	strQuery.Format(_T("SELECT RoomChairs FROM	Rooms WHERE	RoomID = %d"), nRoomID);
	return WriteBlobData<DBA_Room_Chairs>( m_dbSession,strQuery, Data );
}

bool			CCinemaOleDB::IsConnected() const
{
	return m_dbSession.m_spOpenRowset.p != NULL && !m_bForceFailed;
}

void			CCinemaOleDB::GetWorkParam( int nRoomID, int & After, int &Before, int& Period)		
{
	if(!m_dbSession.m_spOpenRowset ) 
	{
		throw TableException("Room table connection error");
	}

	CCommand<CAccessor<DBA_Room_Time> > A_Room;
	HRESULT hr;
	CString sQuery;
	sQuery.Format( _T("SELECT TimeAfterStart, TimeBeforeEnd, TimePeriod FROM Rooms WHERE RoomID=%d"), nRoomID );
	hr = A_Room.Open(m_dbSession, sQuery);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Room table connection error");
	}
	// Get data from the rowset
	if (A_Room.MoveNext() == S_OK)
	{
		After	= A_Room.m_nTimeAfter;
		Before	= A_Room.m_nTimeBefore;
		Period	= A_Room.m_nTimePeriod;
	}		
}

void			CCinemaOleDB::SetWorkParam( int nRoomID, int  After, int Before, int Period)		
{
	if(!m_dbSession.m_spOpenRowset ) 
	{
		throw TableException("Room table connection error");
	}

	CCommand<CAccessor<DBA_Room_Time> > A_Room;
	HRESULT hr;
	CString sQuery;
	sQuery.Format( _T("SELECT TimeAfterStart, TimeBeforeEnd, TimePeriod FROM Rooms WHERE RoomID=%d"), nRoomID );
	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );
	hr = A_Room.Open(m_dbSession, sQuery, &propset );
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Room table connection error");
	}
	// Get data from the rowset
	if (A_Room.MoveNext() == S_OK)
	{
		A_Room.m_nTimeAfter		= After;
		A_Room.m_nTimeBefore	= Before;
		A_Room.m_nTimePeriod	= Period;
		A_Room.SetData();
	}	
}

void	CCinemaOleDB::GetArchiveParam ( int nRoomID, double&  fStart, double &fEnd, int& nDaysKeep )
{
	if(!m_dbSession.m_spOpenRowset ) 
	{
		throw TableException("Room table connection error");
	}

	CCommand<CAccessor<DBA_Room_Backup> > A_Room;
	HRESULT hr;
	CString sQuery;
#if 0
	sQuery.Format( _T("SELECT BackupStart, BackupEnd, BackupDate FROM Rooms WHERE RoomID=%d"), nRoomID );
#else
	sQuery.Format( _T("SELECT BackupStart, BackupStop, BackupDate FROM Rooms JOIN Cinemas ON Rooms.CinemaID = Cinemas.CinemaID WHERE RoomID=%d"), nRoomID );
#endif
	hr = A_Room.Open(m_dbSession, sQuery, 0 );
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Room table connection error");
	}
	// Get data from the rowset
	if (A_Room.MoveNext() == S_OK)
	{
		fStart		= A_Room.m_fStartPercent;
		fEnd		= A_Room.m_fEndPercent;
		nDaysKeep	= A_Room.m_nDaysKeep;
	}
}

void	CCinemaOleDB::SetArchiveParam ( int nRoomID, double  fStart, double fEnd, int nDaysKeep )
{
	if(!m_dbSession.m_spOpenRowset ) 
	{
		throw TableException("Room table connection error");
	}

	CCommand<CAccessor<DBA_Room_Backup> > A_Room;
	HRESULT hr;
	CString sQuery;
#if 0
	sQuery.Format( _T("SELECT BackupStart, BackupEnd, BackupDate FROM Rooms WHERE RoomID=%d"), nRoomID );
#else
	sQuery.Format( _T("SELECT BackupStart, BackupStop, BackupDate FROM Rooms JOIN Cinemas ON Rooms.CinemaID = Cinemas.CinemaID WHERE RoomID=%d"), nRoomID );
#endif
	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );
	hr = A_Room.Open(m_dbSession, sQuery, &propset );
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Room table connection error");
	}
	// Get data from the rowset
	if (A_Room.MoveNext() == S_OK)
	{
		A_Room.m_fStartPercent	= fStart;
		A_Room.m_fEndPercent	= fEnd;
		A_Room.m_nDaysKeep		= nDaysKeep;
		A_Room.SetData();
	}
}

bool		CCinemaOleDB::GetAlgoParam(int nRoomID, std::vector<BYTE>& Data)	
{
	if( !m_dbSession.m_spOpenRowset ) return false;
	CString strQuery;
	strQuery.Format(_T("SELECT AlgoParams FROM	Rooms WHERE	RoomID=%d"), nRoomID);
	return ReadBlobData<DBA_Room_Chairs>( m_dbSession, strQuery, Data );
}

bool		CCinemaOleDB::SetAlgoParam(int nRoomID, const std::vector<BYTE>& Data)
{
	if( !m_dbSession.m_spOpenRowset ) return false;
	CString strQuery;
	strQuery.Format(_T("SELECT AlgoParams FROM	Rooms WHERE	RoomID=%d"), nRoomID);
	return WriteBlobData<DBA_Room_Chairs>( m_dbSession,strQuery, Data );
}

bool		CCinemaOleDB::UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap )
{
	if( !m_dbSession.m_spOpenRowset ) return false;
	bool res = CCinemaDB::UpdateLayout( LayoutMap );
	if( !res) return false;

	CCommand<CAccessor<DBA_Camera> > A_Camera;
	HRESULT hr;
	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );
	hr = A_Camera.Open(m_dbSession, NULL, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Camera table connection error");
	}
	// Get data from the rowset
	while (A_Camera.MoveNext() == S_OK)
	{
		std::map< int, std::pair<int, int > >::const_iterator it = LayoutMap.find( A_Camera.m_nCameraID );
		if( it != LayoutMap.end() )
		{
			A_Camera.m_Coord = CoordPack( it->second );
			hr = A_Camera.SetData( 1 );
		}
	}

	return true;
}

void		CCinemaOleDB::GetMergeModel(int nRoomID, std::vector<BYTE>& Data)
{
	if( !m_dbSession.m_spOpenRowset ) throw DBConnectionException("");
	CString strQuery;
	strQuery.Format(_T("SELECT Model FROM Rooms WHERE RoomID = %d"), nRoomID);
	bool res = ReadBlobData<DBA_Room_Model>( m_dbSession,strQuery, Data );
	if( !res ) throw TableException("");
}

void		CCinemaOleDB::SetMergeModel(int nRoomID, const std::vector<BYTE>& Data)
{
	if( !m_dbSession.m_spOpenRowset ) throw DBConnectionException("");
	CString strQuery;
	strQuery.Format(_T("SELECT Model FROM Rooms WHERE RoomID = %d"), nRoomID);
	bool res = WriteBlobData<DBA_Room_Model>( m_dbSession,strQuery, Data );
	if( !res ) throw TableException("");
}