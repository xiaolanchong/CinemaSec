//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange between CSLauncher & CSChair data exchange protocol implementation
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 23.05.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "RequestImpl.h"
#include "../../CSChair/interfaceEx.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "../syscfg_int.h"
#include "../CSEngine.h"
#include "../res/en/resource.h"
#include "..\Timeouts.h"

#if 1
#include "BoxOffice_i.c"
#else
#include "..\..\CSExchangeTest\CSExchangeTest_i.c"
#endif

#include "MessageProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

EXPOSE_INTERFACE( CRequestImpl, IRequest, REQUEST_INTERFACE );

#define USE_DB
//======================================================================================//
//                                  class RequestImpl                                   //
//======================================================================================//
CRequestImpl::CRequestImpl() : m_State( st_not_running ) 
{
}

CRequestImpl::~CRequestImpl()
{
}

HRESULT	CRequestImpl::Start( IDebugOutput* pInt ) 
{
	m_Debug.Set( pInt );
	ISystemConfig* pSysCfg;
	DWORD dwRes = CreateEngineInterface( SYSTEM_DB_CONFIG_INTERFACE_0,  (void**)&pSysCfg );
	ASSERT( dwRes == 0 );
	pSysCfg->SetDebugInterface( m_Debug.Get() );
	std::vector<int> Rooms;
	HRESULT hr = pSysCfg->GetRoomID(Rooms);
	if( hr != S_OK )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to get RoomID processed list");
		pSysCfg->Release();
		return hr;
	}
	if( Rooms.empty() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"RoomID processed list is empty");
		pSysCfg->Release();
		return E_FAIL;
	}
	m_nRoomID = Rooms[0];

	std::wstring sCS;
	hr = pSysCfg->GetConnectionString(m_sConnestion);
	pSysCfg->Release();
	if( hr != S_OK )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to get connection string, code=%x", hr);
		return hr;
	}
	unsigned int dwId;
	HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, Handled_ThreadProc, this, 0, &dwId  );
	if( reinterpret_cast<uintptr_t>( hThread ) != -1  )
	{
		m_WorkingThread.Start( hThread );
	}
	else
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create a thread");
		return E_FAIL;
	}

	m_Debug.PrintW( IDebugOutput::mt_info, L"CCSRequest started");
	InterlockedExchange( &m_State, st_disconnect );
	return S_OK;
}

HRESULT	CRequestImpl::Stop(  ) 
{
	bool res = m_WorkingThread.Stop( ThreadTimeout_Request );
	m_Debug.PrintW( IDebugOutput::mt_info, res ? 
										L"Exchange thread has been stopped safely" : 
										L"Exchange thread was terminated");
	return S_OK;
}

HRESULT	CRequestImpl::Request( BSTR sXmlQuery ) 
{
	IRequest::State st;
	GetState( st );
	if( st == IRequest::st_not_running )
	{
		m_Debug.PrintW( IDebugOutput::mt_warning, L"Working thread is not working, restarted" );
		Start( m_Debug.Get() );
	}

	AutoLockCriticalSection _al( m_WorkingThreadSync );
	
	if( m_InputQueue.size() > 100 ) return E_PENDING;
	m_InputQueue.push_back( std::make_pair( CTime::GetCurrentTime(), std::wstring(sXmlQuery) ) );
	return S_OK;
}

HRESULT CRequestImpl::GetState( IRequest::State& st )
{
	if( WaitForSingleObject( m_WorkingThread.m_hThread, 100 ) != WAIT_TIMEOUT )
	{
		st = IRequest::st_not_running;
	}
	else
	{
		m_ChangeStateSync.Lock();
		st = (IRequest::State)m_pState->GetStateName();
		m_ChangeStateSync.Unlock();
	}
	return S_OK;
}

unsigned int WINAPI CRequestImpl::Handled_ThreadProc(void * pParam)
{
	unsigned int Res = ~0;
	__try
	{
		CRequestImpl* pThis = (CRequestImpl*)pParam;
		Res = pThis->ThreadProc();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Exchange thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

unsigned int CRequestImpl::ThreadProc()
{
	HRESULT hr = CoInitializeEx( 0, COINIT_MULTITHREADED );
	SimpleTimer timerDBParam(20 * 1000);
	m_ChangeStateSync.Lock();
	m_pState = ExchangeState::GetStartState(this);
	m_ChangeStateSync.Unlock();
	SetDefaultParameters();
	RenewParam();
	timerDBParam.Start();
	while( m_WorkingThread.CanThreadWork( 50 ) )
	{
		if( timerDBParam.IsSignaled() )
		{
			timerDBParam.Start();
			RenewParam();
		}
		m_ChangeStateSync.Lock();
		ExchangeState* pState = m_pState->Process();
		if( pState ) 
		{
			m_pState = boost::shared_ptr<ExchangeState>( pState );
		}
		m_ChangeStateSync.Unlock();
	}
	DisconnectCOM();
	if( hr == S_OK ) CoUninitialize();
	return 0;
}

void	CRequestImpl::PopMessage()
{
	bool bLockRes = m_WorkingThreadSync.Try();
	if( !bLockRes ) return;
	
	if( m_InputQueue.empty() )
	{
		m_WorkingThreadSync.Unlock();
		return;
	}
	MessageQueue_t::iterator it = m_InputQueue.begin();
	for( ; it != m_InputQueue.end();  )
	{
		QueueItem_t& QItem = *it;
		ProcessQuery( QItem.first, QItem.second );
		it = m_InputQueue.erase(it);
	}
	m_WorkingThreadSync.Unlock();
}

bool	CRequestImpl::PopResponseMessage()
{
	bool bLockRes = m_WorkingThreadSync.Try();
	if( !bLockRes ) return false;

	if( m_InputQueue.empty() )
	{
		m_WorkingThreadSync.Unlock();
		return false;
	}
	MessageQueue_t::iterator it = m_InputQueue.begin();
	bool resChange = false;
	for( ; it != m_InputQueue.end();  )
	{
		QueueItem_t& QItem = *it;
		std::pair<bool, bool> res = ProcessQuery( QItem.first, QItem.second );
		if( res.second )
			it = m_InputQueue.erase(it);
		else
			++it;
		if( res.first )
		{
			resChange = true;
			break;
		}
	}
	m_WorkingThreadSync.Unlock();
	return resChange;
}

std::pair<bool, bool>	CRequestImpl::ProcessQuery( CTime timeQuery, const std::wstring& sMsg )
try
{
#if 0
	CString s = timeQuery.Format( _T("%H:%M") );
	LPCWSTR szText = sMsg.c_str();
	m_Debug.PrintW(IDebugOutput::mt_info, L"Message received at %s, text=%s", s, szText );
#endif
	MessageProcessor msg( m_Debug.m_pInt );
	boost::shared_ptr<InputMessage>		in_msg;	
	boost::shared_ptr<OutputMessage>	out_msg;
	bool bDBLog = false;
//	bool bProcessed = false;
	try
	{
		in_msg = msg.ProcessMessage( sMsg );
		m_Debug.PrintW( IDebugOutput::mt_info, L"Operation succeeded" );
		if( in_msg.get() ) 
		{
//			bProcessed = true;
			CCinemaOleDB db;
			db.SetSource( m_sConnestion.c_str(), false );
			db.Open();

			in_msg->Dump( m_Debug );
			LogMessageToDB( db, in_msg->GetDbType(), timeQuery, sMsg  );
			bDBLog = true;
			in_msg->Validate( );
			if( dynamic_cast< WaitForResponseState* > ( m_pState.get() ) )
			{
				bool b = dynamic_cast<BO_ErrorMessage*>( in_msg.get() ) ? true : false;
				return  std::make_pair(b, b) ;
			}
			out_msg = in_msg->Execute( db, m_Debug );
		}
	}
	catch( InvalidQueryException& ex )		{	out_msg=CreateErrorMessage( c_resInvalidQuery,	IDS_SYNTAX_ERROR, ex );	}
	catch( InvalidVersionException& ex )	{	out_msg=CreateErrorMessage( c_resInvalidVersion,IDS_INVALID_VERSION,	ex );	}
	catch( TimeoutException& ex )			{	out_msg=CreateErrorMessage( c_resTimeout,		IDS_TIMEOUT, ex );	}
	catch( InvalidTypeException& ex )		{	out_msg=CreateErrorMessage( c_resInvalidType,	IDS_INVALID_TYPE, ex );	}
	catch( NoDataException& ex)				{	out_msg=CreateErrorMessage( c_resNoData,		IDS_NODATA,	ex );	}
	catch( NoFilmException& ex)				{	out_msg=CreateErrorMessage( c_resNoFilm,		IDS_NOSUCHID,	ex );	}
	catch( FilmAlreadyExistException& ex)	{	out_msg=CreateErrorMessage( c_resFilmAlreadyExists,		IDS_IDEXISTS,	ex );	}
	catch( NoCinemaException& ex)			{	out_msg=CreateErrorMessage( c_resNoCinema,		IDS_WRONGCINEMA, ex );	}
	catch( InvalidDateTimeException& ex)	{	out_msg=CreateErrorMessage( c_resInvalidDateTime,		IDS_WRONGTIME, ex );	}
	catch( QueryDatabaseException& ex)		{	out_msg=CreateErrorMessage( c_resUnknown,		IDS_DATABASE_ERROR, ex );	}
	catch( WholeInfoException& ex)			{	out_msg=CreateErrorMessage( c_resUnknown,		IDS_WHOLE_INFO,	ex );	}
	catch( QueryException& ex )				{	out_msg=CreateErrorMessage( c_resUnknown,		IDS_UNKNOWN_ERROR,	ex );	}
	catch( DataBaseException )					
	{	
		QueryDatabaseException ex( c_DefErrorID );
		out_msg=CreateErrorMessage( c_resUnknown, IDS_DATABASE_ERROR, ex );	
	}
	if( out_msg.get() )
	{
		m_OutputQueue.push( out_msg );
	}
	if( !bDBLog )
	{
		CCinemaOleDB db;
		db.SetSource( m_sConnestion.c_str(), false );
		db.Open();
		LogMessageToDB( db, DBProtocolTable::BO_Unknown, timeQuery, sMsg  );
	}
	return std::make_pair( false, true );
}
catch( QueryException )
{
	m_Debug.PrintW( IDebugOutput::mt_error, L"Error occuried" );
	return std::make_pair(false, true );
};

boost::shared_ptr<OutputMessage>	CRequestImpl::CreateErrorMessage( int nErrorCode, UINT nResID, QueryException& ex )
{
	CString sDesc = LoadStringInternal( nResID );
	boost::shared_ptr<OutputMessage> err(  new CCS_ErrorMessage( ex.GetID(), nErrorCode, std::wstring(sDesc) ) );
	std::wstring s = err->GetText();
	m_Debug.PrintW( IDebugOutput::mt_error, L"[Error output]= %s", s.c_str() );
	return err;
}

void	CRequestImpl::SetDefaultParameters()
{
	m_nExchangeTimeout	= 50;
	m_nExchangeNumber	= 6;
	m_nStatThreshold	= 5 * 60; // 5 min
	m_nWaitTimeout		= 15; // 5 min
}

void CRequestImpl::RenewParam()
{
#ifdef USE_DB
	try
	{
		CCinemaOleDB		db;
		db.SetSource( m_sConnestion.c_str(), false );
		db.Open();
		db.GetExchangeFacet().GetExchangeParam( m_nRoomID, 
			m_nExchangeTimeout, m_nExchangeNumber,
			m_nWaitTimeout, m_nStatThreshold );
		CTime timePrev;
		if( db.GetTableTimetableFacet().GetPreviousCinemaFilm( m_nRoomID, timePrev ) ) 
		{
			CStringW sPrev = timePrev.Format( _T("%d.%m.%Y %H:%M") );
			m_Debug.PrintW( IDebugOutput::mt_debug_info, L"Prev film at %s", (LPCWSTR)sPrev );
			m_timePrevFilm = timePrev;
		}
		m_Debug.PrintW( IDebugOutput::mt_debug_info, 
						L"New params for exchange room %d : attempts=%dsec, timeout=%dsec, wait=%dsec, stat=%dsec",
						m_nRoomID,
						m_nExchangeNumber, m_nExchangeTimeout,
						m_nWaitTimeout, m_nStatThreshold);
	}
	catch( DataBaseException )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Database exception occuried");
	}
#endif
}

bool	CRequestImpl::SendOutputQueue()
{
	HRESULT hr;
	while( !m_OutputQueue.empty() )
	{
		boost::shared_ptr<OutputMessage> out_msg = m_OutputQueue.front();
		ASSERT( out_msg.get() );
		std::wstring s = out_msg->GetText();
		CComBSTR bstrMsg( s.c_str());
		hr = m_pBoxOffice->Request( bstrMsg );
		if( hr == S_OK )
		{
			LogMessageToDB( out_msg->GetDbType(), CTime::GetCurrentTime(), s );
			m_OutputQueue.pop();
		}
		else return false;
	}
	return true;
}

bool	CRequestImpl::ConnectCOM()
{
	HRESULT hr = m_pBoxOffice.CoCreateInstance( CLSID_BoxOfficeRequest);
	if( hr != S_OK )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create IID_IBoxOfficeRequest" );
		return false;
	}
	else return true;
}

void	CRequestImpl::DisconnectCOM()
{
	m_pBoxOffice.Release();
}

bool	CRequestImpl::IsTimeForStatReport( int nPeriodSec)
{
#if 1
	if( !m_timePrevFilm ) return false;
	CTime timePrevFilm = m_timePrevFilm.get();
	CTimeSpan LowerBound( 0, 0, 0, m_nStatThreshold );
	CTimeSpan UpperBound( 0, 0, 0, m_nStatThreshold + nPeriodSec );
	return (	CTime::GetCurrentTime() >=	timePrevFilm + LowerBound &&
				CTime::GetCurrentTime() <	timePrevFilm + UpperBound  );
#else // test
	static SimpleTimer z(200 * 1000);
	static bool bFirst = true;
	if( bFirst )
	{
		bFirst = false;
		z.Start();
		return true;
	}
	else if( z.IsSignaled() )
	{
		z.Start();
		return true;
	}
	else return false;
#endif
}

bool	CRequestImpl::CreateStatReport()
try
{
	CCinemaOleDB		db;
	db.SetSource( m_sConnestion.c_str(), false );
	db.Open();
	std::vector<FilmStat_t> StatArr;
	int nStatID;
	const int nHourStat = 60 * 24; //24h
	CTime timeLower = CTime::GetCurrentTime() - CTimeSpan( 0, nHourStat, 0, 0);
	CTime timeUpper = CTime::GetCurrentTime();
	db.GetTableStatFacet().GetStatForExchange( m_nRoomID, timeLower, timeUpper, StatArr, nStatID );

	MessageProcessor msg( m_Debug.m_pInt );
	boost::shared_ptr<OutputMessage>	out_msg ( new CCS_ReportMessage( nStatID, timeUpper, StatArr )) ;
	m_OutputQueue.push( out_msg );
	return true;
}
catch( DataBaseException )
{
	m_Debug.PrintW( IDebugOutput::mt_error, L"[CreateStatReport]Database exception occuried");
	return false;
};


HRESULT CRequestImpl::Reconnect()
{
	AutoLockCriticalSection al( m_ChangeStateSync );
	if( dynamic_cast<DisconnectState*>  ( m_pState.get() ) )
	{
		m_pState = boost::shared_ptr< ExchangeState >  ( new ConnectState( this ) );
		return S_OK;
	}
	else return E_ALREADY_CONNECTED;
}

HRESULT CRequestImpl::ForceReport()
{
	AutoLockCriticalSection al( m_ChangeStateSync );
	if( dynamic_cast<SendReportState*>		( m_pState.get() ) ||
		dynamic_cast<IncomingRequestState*> ( m_pState.get() )    )
	{
		m_pState = boost::shared_ptr< ExchangeState >  ( new SendReportState( this ) );
		return S_OK;
	}
	else return E_NOT_CONNECTED;
}

void	CRequestImpl::LogMessageToDB( DBProtocolTable::MsgType mt, CTime time, const std::wstring& sText )
{
	CCinemaOleDB db;
	db.SetSource( m_sConnestion.c_str(), false );
	db.Open();
	LogMessageToDB(db, mt, time, sText ); 
}

void	CRequestImpl::LogMessageToDB( CCinemaOleDB& db, DBProtocolTable::MsgType mt, CTime time, const std::wstring& sText )
try
{
	db.GetProtocolTable().AddMessage( m_nRoomID, mt, time, sText );
}
catch ( ... ) 
{
	m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to log message in DB" );
};
