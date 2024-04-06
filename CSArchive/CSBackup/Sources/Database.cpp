/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Database.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exception.hpp"
#include "Utility.hpp"
#include "Database.hpp"

void DataSourceDestructor( CDataSource* p )
{
    if( p )
	{
		p->Close();
		delete p;
	}
}

CCinemaDatabase::CCinemaDatabase( const std::wstring& sConnectionString )
{
	assert( !sConnectionString.empty() );
    m_sConnectionString = sConnectionString;
}

CCinemaDatabase::~CCinemaDatabase()
{

}

//////////////////////////////////////////////////////////////////////////
void CCinemaDatabase::Connect( )
{
	m_pDataSource = boost::shared_ptr<CDataSource>( new CDataSource(), DataSourceDestructor );

	HRESULT hr; 
	hr = m_pDataSource->OpenFromInitializationString( m_sConnectionString.c_str() );
	if( FAILED(hr) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat(L"Can't open data source from the initialization string, error: %1%" ) % hr ) );
}

void CCinemaDatabase::Disconnect()
{
	m_pDataSource.reset();
}

//////////////////////////////////////////////////////////////////////////
/* Get camera ids */
void CCinemaDatabase::GetCameraIDs( std::wstring& sHostIP, std::vector<int>& ids )
{
	HRESULT hr;

	CSession session;

	//Open DB session
	hr = session.Open( *(m_pDataSource.get()) );
	if( FAILED(hr) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat(L"Can't open session, error: %1%" ) % hr ) );

	//If an exception raised, session will be closed correctly
	boost::shared_ptr<CSession> GuardSession( &session, boost::mem_fn( &CSession::Close ) );

	//Format SQL command
	std::wstring sCommand = boost::str( boost::wformat( L"SELECT CameraID \
															FROM Cameras \
															WHERE CameraIP=\'%1%\';" )  % sHostIP );

	CCommand<CAccessor< CCinemaDatabase::CCamerasAccessor > > AccCamsAmount;

	//Retrieve rows 
	hr = AccCamsAmount.Open( session, sCommand.c_str() );
	if(FAILED( hr ) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't execute the sql command: %1%, error: %2%" ) % sCommand % hr ) );

	boost::shared_ptr< CCommand<CAccessor< CCinemaDatabase::CCamerasAccessor > > > 
		GuardCommand( &AccCamsAmount, boost::mem_fn( &CCommand<CAccessor< CCinemaDatabase::CCamerasAccessor > >::Close ) );

	//Fetch values
	for( hr = AccCamsAmount.MoveFirst(); hr != DB_S_ENDOFROWSET; hr = AccCamsAmount.MoveNext() )
	{
		if( FAILED(hr) )
			throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't fetch values from the database, error: %1%" ) % hr ) );
		ids.push_back( AccCamsAmount.m_CameraID );
	}
}

/* Get storage cleaning params */
void CCinemaDatabase::GetCleaningParams( const std::vector<DWORD>& roomIds, double& dBackupStart, double& dBackupStop, DATE& LastDate  )
{
	HRESULT hr;
	CSession session;

	//Open DB session
	hr = session.Open( *(m_pDataSource.get()) );
	if( FAILED(hr) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat(L"Can't open session, error: %1%;" ) % hr ) );

	//If an exception raised, session will be closed correctly
	boost::shared_ptr<CSession> GuardSession( &session, boost::mem_fn( &CSession::Close ) );

	std::wstring sCommand = 
		boost::str( boost::wformat( L"SELECT BackupStart, BackupStop, BackupDate \
										FROM Cinemas INNER JOIN Rooms \
										ON Cinemas.CinemaID=Rooms.CinemaID \
										WHERE Rooms.RoomID=%1%" )  % roomIds[0] );


	CCommand<CAccessor< CCinemaDatabase::CCinemaAcc > > AccCinemas;

	//Retrieve rows
	hr = AccCinemas.Open( session, sCommand.c_str() );
	if(FAILED( hr ) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't execute the sql command %1%, error: %2%" ) % sCommand % hr ) );

	boost::shared_ptr< CCommand<CAccessor< CCinemaDatabase::CCinemaAcc > > > 
		GuardCommand( &AccCinemas, boost::mem_fn( &CCommand<CAccessor< CCinemaDatabase::CCinemaAcc > >::Close ) );

	//Fetch value
	hr = AccCinemas.MoveFirst(); 
	if( FAILED(hr) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't fetch values from the database, error: %1%" )  % hr ) );

	//Assign values
	dBackupStart = AccCinemas.m_BackupStart;
	dBackupStop = AccCinemas.m_BackupStop;

	//Get last check time
	COleDateTime dtCurrentDate( COleDateTime::GetCurrentTime() );
	COleDateTime dtLastDate( dtCurrentDate - COleDateTimeSpan( AccCinemas.m_BackupDate, 0,0,0 ) );
	
	LastDate = static_cast<DATE>( dtLastDate );
}

/* Update DB file statistics */
void CCinemaDatabase::UpdateStatistics( const std::vector<DWORD>& roomIds, DATE& upToDate )
{
	HRESULT hr;

	//Convert date to string
	CString sUpToDate = COleDateTime( upToDate ).Format( L"%Y-%m-%d %H:%M:%S" );

	CSession session;
	//Open DB session
	hr = session.Open( *(m_pDataSource.get()) );
	if( FAILED(hr) )
		throw CException<CCinemaDatabase>( boost::str( boost::wformat(L"Can't open session, error: %1%" ) % hr ) );

	//If an exception raised, session will be closed correctly
    boost::shared_ptr<CSession> GuardSession( &session, boost::mem_fn( &CSession::Close ) );

	//Update statistics for [Statistics]
	for( std::vector<DWORD>::const_iterator roomIt = roomIds.begin(); roomIt != roomIds.end(); ++roomIt )
	{
		//Format SQL command
		std::wstring sCommand = 
			boost::str( boost::wformat( L"DELETE FROM [Statistics] "
										L"WHERE (StatTime<'%s') AND (RoomId=%d);" ) % (LPCWSTR)sUpToDate % *roomIt );

		CCommand<CNoAccessor> AccCamsAmount;

		CDBPropSet	propset(DBPROPSET_ROWSET);
		propset.AddProperty(DBPROP_IRowsetChange, true);
		propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_DELETE);

		//Retrieve rows 
		hr = AccCamsAmount.Open( session, sCommand.c_str(), &propset );
		if(FAILED( hr ) )
			throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't execute the sql command: %1%, error: %2%" ) % sCommand % hr ) );

		boost::shared_ptr< CCommand<CNoAccessor> > 
			GuardCommand( &AccCamsAmount, boost::mem_fn( &CCommand<CNoAccessor>::Close ) );

	}

	//Update statistics for [Timetable]
	for( std::vector<DWORD>::const_iterator roomIt = roomIds.begin(); roomIt != roomIds.end(); ++roomIt )
	{
		//Format SQL command
		std::wstring sCommand = 
			boost::str( boost::wformat( L"DELETE FROM [Timetable] \
										 WHERE (EndTime<'%s') AND (RoomId=%d);" ) % (LPCWSTR)sUpToDate % *roomIt );

		CCommand<CNoAccessor> AccCamsAmount;

		//Retrieve rows 
		hr = AccCamsAmount.Open( session, sCommand.c_str() );
		if(FAILED( hr ) )
			throw CException<CCinemaDatabase>( boost::str( boost::wformat( L"Can't execute the sql command: %1%, error: %2%" ) % sCommand % hr ) );

		boost::shared_ptr< CCommand<CNoAccessor> > 
			GuardCommand( &AccCamsAmount, boost::mem_fn( &CCommand<CNoAccessor>::Close ) );

	}

}