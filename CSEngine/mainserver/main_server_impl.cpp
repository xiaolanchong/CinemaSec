#include "stdafx.h"

using std::max;
using std::min;

#include "main_server_impl.h"
#include <process.h>
#include <Shlwapi.h> 


#include "../ipc/NetUtil.h"
#include "../ipc/NetAddr.h"
#include "../CSEngine.h"
#include "../../CSChair/public/NameParse.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "ServerTimeAction.h"
#include "../syscfg_int.h"
#include "..\Timeouts.h"

//#include <locale>
//last header
#include "../../CSChair/public/memleak.h"

#pragma warning( disable : 4355 )

MACRO_EXCEPTION(MainServerException, ThreadServerException)
MACRO_EXCEPTION(MainServerInitException, MainServerException)

CMainServerImpl::CMainServerImpl():
	m_State(ss_begin),
	m_HallConfig( ),
	m_NetProtocol(this),
	m_nRoomID(-1),
	m_bUseTimeTable(true)
	,m_pNetworkInt(0)
{
	m_pTimeAction = std::auto_ptr<TimeAction>( new StaStopped(*this) );
}

CMainServerImpl::~CMainServerImpl()
{
	DWORD res;
	if( m_State == ss_running) 
		res = StopWork(true);
	res = StopServer();
}

void CMainServerImpl::Release()
{
	delete this;
}

struct BoolResValid
{
	void operator = (bool res)
	{
		if(!res) throw MainServerException("Boolean result assertion");
	}
};

/////////// CONFIG //////////////////////////////

DWORD	CMainServerImpl::GetDBConfig()
{
	// FIXME to IStatisticsServer
	HRESULT res;
	HRESULT hr;
	ISystemConfig* pCfgInt ;
	res = CreateEngineInterface( SYSTEM_DB_CONFIG_INTERFACE_0, (void**)&pCfgInt );
	if( res )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to create config interface" );
		return E_FAIL;
	}
	boost::shared_ptr<ISystemConfig> pInt( pCfgInt, DestroySystemConfigInt );
	
	pCfgInt->SetDebugInterface( m_pDebug.Get() );
	std::vector<int> RoomIDArr;
	hr = pInt->GetRoomID(RoomIDArr);
	int nRoomID;
	try
	{
		nRoomID = StartRoomSession( RoomIDArr );
	}
	catch( SessionServerUnexpectedException )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Error occured while trying to detect required RoomID" );
		return E_FAIL;
	}
	catch( SessionServerNoEmptyException )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"No suitable room, server will not be initialized" );
		return E_FAIL;
	}
	catch(SessionServerException)
	{
		return E_FAIL;
	}
	m_nRoomID = nRoomID;
	hr = pInt->LoadFromDB( false, true );
	if( hr )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to load config from db, reason=%x", hr );
		return E_FAIL;
	}	
	bool						bLocalHall;
	hr = pInt->GetLocalCamera( nRoomID, m_LocalCameras );
	if( hr != S_OK )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to load local camera config, reason=%x", hr );
		return E_FAIL;
	}
	hr = pInt->GetRemoteCamera( nRoomID, m_RemoteCameras );
	if( hr != S_OK )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to load remote camera config, reason=%x", hr );
		return E_FAIL;
	}
	hr = pInt->GetHallConfig( nRoomID, m_HallConfig, m_HallProcessConfig, bLocalHall );
	if( hr != S_OK )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to load hall config, reason=%d", hr );
		return E_FAIL;
	}
	hr = pInt->GetNetConfig( nRoomID, m_NetConfig );
	if( hr != S_OK )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to load network config, reason=%x", hr );
		return E_FAIL;
	}
	return S_OK;
}
#if 0
DWORD	CMainServerImpl::SetDebugWindow(HWND hWnd) 
{
	if( m_State == ss_running )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server must be stopped");
		return ERR_ERROR;
	}
	return ERR_NOTIMPL;
}
#endif
DWORD	CMainServerImpl::SetDebugInterface(IDebugOutput* pDebugInt)
{
	if( m_State == ss_running )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server must be stopped");
		return ERR_ERROR;
	}
	m_pDebug.Set( pDebugInt );
	return ERR_OK;
}

HRESULT		CMainServerImpl::SetCameraWindow(int nCameraNo, HWND hWnd) 
{
	for( size_t i = 0; i < m_LocalCameras.size(); ++i )
	{
		if( m_LocalCameras[i].m_nCameraNo == nCameraNo )
		{
			m_CameraWindows[ nCameraNo ] = hWnd;
		}
	}
	return E_FAIL;
}
/*
HRESULT		CMainServerImpl::SetHallWindow( HWND hWnd) 
{
	if( !m_HallConfig.IsLocal() ) return E_FAIL;
	return S_OK;
}
*/
/////////// INITIALIZE //////////////////////////////
DWORD	CMainServerImpl::Initialize( IGrabberFactory* pFactory, INetworkLayer* pNetInt )
{
	m_bUseTimeTable	= true;
	DWORD res;
	if( m_State == ss_running )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server must be stopped");
		return ERR_ERROR;
	}
	if(  m_State == ss_initialized || m_State == ss_stopped )
	{
		DWORD dwRes = StopServer();
	}

	if( !pNetInt  )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Invalid network layer");
		return ERR_INVALIDARG;
	}
	m_pNetworkInt = pNetInt;

	if(  !pFactory )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Invalid grabber factory");
		return ERR_INVALIDARG;
	}
	m_pGrabberFactory = pFactory;
	
	DWORD dwRes = GetDBConfig();
	if(  dwRes != S_OK ) return dwRes;
	// statistics on all computers
	IStatisticsServer* pStatInt;
	res = CreateEngineInterface( STATISTICS_SERVER_INTERFACE_0, (void**)&pStatInt );
	m_StatServer = boost::shared_ptr<IStatisticsServer>( pStatInt, DestroyStatisticsServer );
	if( !pStatInt )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to create statistics server instance");
		return ERR_ERROR;
	}
	else
	{
		res = m_StatServer->Start( /*NULL*/ );
		m_StatServer->SetDebugInterface( m_pDebug.Get() );
		if( res != S_OK )
		{
			m_StatServer.reset();
			m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to start statistics server");
			return ERR_ERROR;
		}
	}

	dwRes = StartServer();
	if( !dwRes ) 
	{
		//m_pDebug.PrintW(IDebugOutput::mt_info,L"Server initialized");
		InterlockedExchange( &m_State, ss_initialized );
		//return ERR_OK;
	}
		
	res = InitializeWork();

	return res;
}

DWORD	CMainServerImpl::InitializeWork()
{
	if( m_State == ss_begin)
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,
				L"Server does not initialize a network connection properly");
		return ERR_ERROR;
	}
	if( m_State == ss_running    )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server must be stopped");
		return ERR_ERROR;
	}
#if 0
	m_CameraAnalyzers.clear();
#endif
	m_HallAnalyzer.reset();

	if( m_HallConfig.IsLocal())
		m_pDebug.PrintW(IDebugOutput::mt_info,
			L"Server initialized: room=%d (%d local camera(s), %d remote camera(s), local hall analyzer)",
			m_nRoomID,
			m_LocalCameras.size(), m_RemoteCameras.size()  );
	else
		m_pDebug.PrintW(IDebugOutput::mt_info,
			L"Server initialized: room=%d (%d local camera(s), %d remote camera(s), hall analyzer at %s)",
			m_nRoomID,
			m_LocalCameras.size(), m_RemoteCameras.size(), m_HallConfig.m_sHallUrl.c_str() );

	InterlockedExchange( &m_State, ss_stopped );
	return ERR_OK;
}

/////////// START //////////////////////////////

DWORD	CMainServerImpl::Start	(bool bForce)
{
	if( m_bUseTimeTable )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Cannot start when use db timetable");
		return ERR_ERROR;
	}
	else
		return StartWork( bForce );
}

DWORD	CMainServerImpl::Stop	(bool bForce)	
{
	if( !bForce && m_bUseTimeTable)
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Cannot stop when use db timetable");
		return ERR_ERROR;
	}
	else
		return StopWork( bForce );
}

DWORD	CMainServerImpl::StartWork	(bool bForce)	
{
	if( m_State == ss_running )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server already is running");
		return ERR_ERROR;
	}
	if( m_State != ss_stopped)
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server is not initialized");
		return ERR_ERROR;
	}

	if( !m_HallConfig.IsLocal() && !bForce)
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server can not be started manually");
		return ERR_ERROR;
	}

	DWORD res;

	if( m_HallConfig.IsLocal() )
	{
		// get own timetable
		SetFilmTime( m_TimeSheduler.GetFilmTime() );
	}
	
	RefreshAlgoParam();
	res = StartCameraThreads( GetFilmTime().first, GetFilmTime().second );
	if( res != S_OK )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Failed to start camera threads" );
		return res;
	}
	if(m_HallConfig.IsLocal()  )
	{
		res = StartHallThread( m_HallConfig.m_Chairs, m_HallConfig.m_AlgoParam, 
								GetFilmTime());
		if(res != S_OK) 
		{
			StopCameraThreads();
			return res;
		}
		if( m_HallAnalyzer.get() )
			SendStartWork( m_NetProtocol, GetFilmTime() );
		m_HallAnalyzer->m_CameraData.clear();
		for( size_t i = 0; i < m_LocalCameras.size(); ++i )
		{
			m_HallAnalyzer->m_CameraData[ m_LocalCameras[i].m_nCameraNo ] = CameraData();
		}

		for( size_t i = 0; i < m_RemoteCameras.size(); ++i )
		{
			m_HallAnalyzer->m_CameraData[ m_RemoteCameras[i].m_nCameraNo ] = CameraData();
		}
	}
	else
	{
		m_pDebug.PrintW(IDebugOutput::mt_info,L"Use remote hall analyzer=%s", m_HallConfig.m_sHallUrl.c_str() );
	}
	if( m_HallAnalyzer.get() )
	{
		m_HallAnalyzer->m_Thread.Resume();
	}
	//m_MainThread.Resume();

	m_pDebug.PrintW( IDebugOutput::mt_info, L"Server started");

	InterlockedExchange( &m_State, ss_running );
	return ERR_OK;
}

DWORD	CMainServerImpl::StartServer()
{
	try
	{
		unsigned long Group = m_NetConfig.m_dwMultiCastGroup;
	}
	catch( NetException& ex )
	{
		m_pDebug.PrintA( IDebugOutput::mt_error, ex.what() );
		return ERR_ERROR;
	}
	unsigned int dwId;

	HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, Handled_ServerThreadProc, this, /*CREATE_SUSPENDED*/0, &dwId  );
	if( reinterpret_cast<uintptr_t>( hThread ) != -1  )
	{
		m_MainThread.Start( hThread );
	}
	else
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to create a server thread");
	}
	//m_pDebug.PrintW( IDebugOutput::mt_error, L"Server established the network connection");
	return ERR_OK;
}

/////////// STOP //////////////////////////////

DWORD	CMainServerImpl::StopWork	(bool bForce)	
{
	if( m_State != ss_running )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server is not running");
		return E_FAIL;
	}

	if( !m_HallConfig.IsLocal() && !bForce)
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Server can not be stoppped manually");
		return ERR_ERROR;
	}

	if( m_HallAnalyzer.get() )
		SendStopWork( m_NetProtocol );

	DWORD res;
	InterlockedExchange( &m_State, ss_stopped );
	m_AnalyzerCS.Lock();
	res = StopHallThread();
	res = StopCameraThreads();
	m_AnalyzerCS.Unlock();

	return ERR_OK;
}

DWORD	CMainServerImpl::StopServer()
{
	bool res = m_MainThread.Stop( ThreadTimeout_MainServer );
	if(res)
		m_pDebug.PrintW(IDebugOutput::mt_info,L"Server stopped safely");
	else
		m_pDebug.PrintW(IDebugOutput::mt_warning,L"Server terminated");
	try
	{
	}
	catch( NetException& ex )
	{
		m_pDebug.PrintA( IDebugOutput::mt_error, ex.what() );
	}
	return ERR_OK;
}	

/////////////// OTHERS /////////////////////////////////

ICameraAnalyzer*	CMainServerImpl::GetCameraAnalyzer(int CameraNo)	
{
	return CameraServer::GetCameraAnalyzer(CameraNo);
}

IHallAnalyzer*		CMainServerImpl::GetHallAnalyzer()	
{
	return   HallServer::GetHallAnalyzer();
}

//////////////// THREADPROC ///////////////////////////////
unsigned int WINAPI CMainServerImpl::Handled_ServerThreadProc(void * pParam)
{
	unsigned int Res = err_exception;
	__try
	{
		Res = ServerThreadProc(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Main server thread"))
#ifndef _DEBUG
	 , EXCEPTION_EXECUTE_HANDLER
#endif
	)
	{
	}
	return Res;
}

//bool IsGetCameraDataSignalled


unsigned int  WINAPI CMainServerImpl::ServerThreadProc( void * pParam )
{
	const DWORD		c_timeThreadTimer		= 20;		//ms
	CMainServerImpl* pThis = (CMainServerImpl*)pParam;

	//FIXME: TO_DB_CONFIG
	AutoSimpleTimer timerStatisticsPump(10 * 1000);
	timerStatisticsPump.Start();

	SimpleTimer timerForCurrentState(1 * 1000); // 1s
	timerForCurrentState.Start();

	while( pThis->m_MainThread.CanThreadWork( c_timeThreadTimer ) )
	{
		if( pThis->m_State == ss_running )
		{
			// sync check thread and stop thread functions
			AutoLockCriticalSection al( pThis-> m_AnalyzerCS ); 
			pThis->CheckChildThreads();
		}
		else
		{
		}
		pThis->PumpMessage(pThis->m_NetProtocol);
		pThis->PumpImage();
		// use timetable when only all devices is ready
		if( pThis->m_bUseTimeTable  && 
			(	pThis->m_State == ss_stopped  ||
				pThis->m_State == ss_running  )		)
		{
			if( timerStatisticsPump.IsSignaled() )
				pThis->PumpState();
			else
			{
				pThis->m_TimeSheduler.ProcessNoSetting( pThis->m_pTimeAction.get() );
			}
		}
		if( timerForCurrentState.IsSignaled() )
		{
			timerForCurrentState.Start();
			pThis->SendCurrentState();
		}

		pThis->m_pTimeAction->Exec();
	}

	RETURN(err_noerror) ;
}

void CMainServerImpl::CheckChildThreads()
{
	CheckCameraThreads();
	if(  m_HallAnalyzer.get() )
	{
		std::pair<bool, DWORD> stat = m_HallAnalyzer->m_Thread.IsThreadEnds();
		if( stat.first )
		{
			m_pDebug.PrintW( IDebugOutput::mt_error, L"Hall thread exits with code=%d, desc=%s", 
				stat.second, 
				GetThreadErrorDescription(stat.second) );
		}		
	}
}

void	CMainServerImpl::GetDataFromCameras( const std::set<int>& Cameras)
{
#define USE_NETWORK_FOR_TRANSITION
	std::vector<int>	LocalIndex;
	std::set<int>		RemoteIndex;
	std::map < std::wstring, std::vector<int> > Host2Cameras;


	//split to local & remote cameras
	for(size_t i = 0 ; i < m_LocalCameras.size(); ++i)
	{
		int nCameraNo = m_LocalCameras[i].m_nCameraNo;
		if( Cameras.find(nCameraNo) != Cameras.end() )
		{
			LocalIndex.push_back( nCameraNo );
		}
	}
#ifdef USE_NETWORK_FOR_TRANSITION
	std::wstring sOwnName = GetOwnName();
	if( !LocalIndex.empty() ) 
		Host2Cameras[ sOwnName ] = LocalIndex;
#endif

	for(size_t i = 0 ; i < m_RemoteCameras.size(); ++i)
	{
		int nCameraNo = m_RemoteCameras[i].m_nCameraNo;
		if( Cameras.find(nCameraNo) != Cameras.end() )
		{
			RemoteIndex.insert( nCameraNo );
		}
		std::wstring sHostName = m_RemoteCameras[i].m_sRemoteUrl;

		std::locale loc1 ( "English_US" );
		std::use_facet<std::ctype<wchar_t> > ( loc1 ).tolower ( &sHostName[0], & (*sHostName.end()) );

		Host2Cameras[ sHostName ].push_back( nCameraNo );
	}

#ifndef USE_NETWORK_FOR_TRANSITION
	for(size_t i = 0; i < LocalIndex.size(); ++i )
	{
		int CameraNo = LocalIndex[i];
		PushImageThreadMessage ( CameraNo  );
	}
#endif
	SendCameraRequest(m_NetProtocol, Host2Cameras);
}

void	CMainServerImpl::LoadDataToHallAnalyzer()
{
	SetEvent(m_HallAnalyzer->m_hEventForProcessing);
}

///////////// NETWORK ////////////////////////////////////

void	CMainServerImpl::SendStartWork(NetProto& Np , const std::pair<CTime, CTime>& TimeFilm)
{
	netadr_t NetAddress;
	struct sockaddr_in s;
	s.sin_family	= AF_INET;
	s.sin_addr.s_addr = m_NetConfig.m_dwMultiCastGroup;
	s.sin_port = htons( m_NetConfig.m_wUDPServerPort );
	SockadrToNetadr((sockaddr*) &s, &NetAddress );
	m_pDebug.PrintW( IDebugOutput::mt_info, L"Send StartWork to %s", NET_AdrToString( NetAddress ).c_str() );

	SessionServer::StartServerSession();
	NetStart Ns( SessionServer::GetSessionID(), TimeFilm.first.GetTime(), TimeFilm.second.GetTime() ) ;
	Np.PushPacket( NetAddress, &Ns, false );
}

void	CMainServerImpl::SendStopWork(NetProto& Np)
{
	netadr_t NetAddress;
	struct sockaddr_in s;
	s.sin_family	= AF_INET;
	s.sin_addr.s_addr = m_NetConfig.m_dwMultiCastGroup;
	s.sin_port = htons( m_NetConfig.m_wUDPServerPort );
	SockadrToNetadr((sockaddr*) &s, &NetAddress );
	m_pDebug.PrintW( IDebugOutput::mt_info, L"Send StopWork to %s", NET_AdrToString( NetAddress ).c_str() );

	NetStop Ns;
	Np.PushPacket( NetAddress, &Ns, false );
}

void	CMainServerImpl::SendCameraRequest(NetProto& Np, const std::map < std::wstring, std::vector<int> >& Host2Cameras )
{
	std::map < std::wstring, std::vector<int> >::const_iterator it =
			Host2Cameras.begin();
	for(; it != Host2Cameras.end(); ++it)
	{
		const std::wstring&			sName	= it ->first;
		const std::vector<int>&		Cameras = it->second;
		netadr_t NetAddress;
		NET_StringToAdr( sName.c_str(), &NetAddress );
		NetAddress.port = NET_HostToNetShort( m_NetConfig.m_wTCPServerPort );
		m_pDebug.PrintW( IDebugOutput::mt_info, L"Send DataRequest to %s", sName.c_str() );
		NetRequest NRequest( Cameras );
		Np.PushPacket( NetAddress, &NRequest, true );
	}
}

void	CMainServerImpl::PumpMessage( NetProto& Np )
{
	m_pNetworkInt->LockIncomeBuffer( m_nRoomID );
	NetIncomeBuffer_t& MsgBuf = m_pNetworkInt->GetIncomeBuffer( m_nRoomID );
	NetIncomeBuffer_t::const_iterator it = MsgBuf.begin();
	for( ; it != MsgBuf.end(); ++it )
	{
		if( MsgBuf.size() == 0 ) DebugBreak();
		Np.PumpMessage( it->first, it->second );
	}
	MsgBuf.clear();
	m_pNetworkInt->UnlockIncomeBuffer( m_nRoomID );
}

void	CMainServerImpl::TrySendDataResponse()
try
{
	if( !m_ClientCameraData.IsAllRequired() ) return;

	NetResponse NResponse;
	const std::vector<int>& Cams = m_ClientCameraData.GetCameras();
	for( size_t i = 0; i < Cams.size()  ; ++i)
	{
		const std::vector<BYTE>	& Data	= m_ClientCameraData.GetData( Cams[i] );
		const std::vector<BYTE>	& Img	= m_ClientCameraData.GetImage( Cams[i] );
		NResponse.AddCamera( Cams[i] , 0, Data, Img );
	}
	netadr_t na = m_ClientCameraData.GetNetAddress();
	netadr_t TrueNetAddress(na);
	// bugfix 2005-07-05 change server host port
	TrueNetAddress.port = NET_HostToNetShort( m_NetConfig.m_wTCPServerPort );
	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"SendDataResponse sends data to %s", NET_AdrToString(TrueNetAddress).c_str() );
	m_NetProtocol.PushPacket(TrueNetAddress, &NResponse, true);
	m_ClientCameraData.Clear();
}
catch( CommonException )
{
	//	ASSERT(FALSE);
	m_pDebug.PrintW( IDebugOutput::mt_error, L"TrySendDataResponse exception's error" );
};

void	CMainServerImpl::SendCurrentState()
{
	int nServerState;
	if( m_State == ss_stopped) nServerState = CurrentStateType::State_Stopped;
	else if( dynamic_cast< StaProcessed *>( m_pTimeAction.get() ) )
	{
		nServerState = CurrentStateType::State_Processing;
	}
	else if( m_State == ss_running)
	{
		nServerState = CurrentStateType::State_Working;
	}
	else if(m_State == ss_stopped) ;
	else nServerState = CurrentStateType::State_Unknown;

	NetCurrentState NetCurState( m_nRoomID, nServerState,  GetSessionID(),
								 GetFilmTime().first.GetTime(), GetFilmTime().second.GetTime()
								 );

	int nCameraState;
	for( size_t i = 0 ; i < m_LocalCameras.size(); ++i )
	{
		int nCameraNo = m_LocalCameras[i].m_nCameraNo;
		ICameraAnalyzer* pThread = GetCameraAnalyzer(nCameraNo);
#if 0
		nCameraState = pThread? nServerState : CurrentStateType::State_Dead;
#else
		nCameraState = CurrentStateType::State_Working;
#endif
		NetCurState.AddCamera( nCameraNo, nCameraState );
	}
	if( IsMainServer() )
	{
		HallThread* pThread = m_HallAnalyzer.get();
		int nHallState = pThread? nServerState : CurrentStateType::State_Dead;
		NetCurState.AddHall( nServerState );
	}
	netadr_t NetAddress;
	struct sockaddr_in s;
	s.sin_family	= AF_INET;
	s.sin_addr.s_addr = m_NetConfig.m_dwMultiCastGroup;
	s.sin_port = htons( m_NetConfig.m_wTCPServerPort );
	SockadrToNetadr((sockaddr*) &s, &NetAddress );
#ifdef PRINT_CURRENT_STATE
	m_pDebug.PrintW( IDebugOutput::mt_info, L"Send CurrentState to %s", NET_AdrToString( NetAddress ).c_str() );
#endif
	// through UDP interface to multicast
	m_NetProtocol.PushPacket( NetAddress, &NetCurState, false );
}

////// callbacks, incoming messages //////////////

void CMainServerImpl::OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ) 
{
	if( m_State != ss_running ) 
	{
		m_pDebug.PrintW( IDebugOutput::mt_warning, 
						L"Received DataRequest from %s, but server is not running", NET_AdrToString( NetAddr ).c_str() ); 
		m_pNetworkInt->TCP_CloseClient(m_nRoomID, NetAddr );
		return; 
	}
	netadr_t OwnAddr;
	NET_GetOwnAddress( m_NetConfig.m_wTCPServerPort, &OwnAddr );
	m_pDebug.PrintW( IDebugOutput::mt_info, L"RequestData from %s", NET_AdrToString(NetAddr).c_str() );

	m_ClientCameraData.NewRequest( NetAddr, CameraIDs );
	m_pNetworkInt->TCP_CloseClient( m_nRoomID,NetAddr );

	for(  size_t i = 0; i < CameraIDs.size(); ++i )
	{
		CameraThread* pThread = FindCameraThread( CameraIDs[i] );
		if( !pThread )
			m_pDebug.PrintW( IDebugOutput::mt_error, L"No such requested camera=%d", CameraIDs[i] );
		else
		{
			PushImageThreadMessage( CameraIDs[i] );
			m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"PushImageThreadMessage camera=%d", CameraIDs[i] );
		}
	}
	///
}

void CMainServerImpl::OnResponseData( netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data )
{
	if( !m_HallAnalyzer.get() || m_State != ss_running )
	{
		m_pDebug.PrintW( IDebugOutput::mt_warning, 
			L"Received DataResponse from %s, but server is not running", NET_AdrToString( NetAddr ).c_str() );
		m_pNetworkInt->TCP_CloseClient( m_nRoomID,NetAddr );
		return; 
	}
	m_HallAnalyzer->m_SyncCS.Lock();
	for( size_t i = 0; i < Data.size()  ; ++i)
	{
		const std::vector<BYTE>& DataArr = Data[i].Data;
		const std::vector<BYTE>& ImgArr = Data[i].Img;
		int nCameraNo = Data[i].nCameraNo;
		
		WCHAR szDataSizeBuf[100], szImgSizeBuf[100];
		StrFormatByteSizeW( DataArr.size(), szDataSizeBuf, 100 );
		StrFormatByteSizeW( ImgArr.size(), szImgSizeBuf, 100 );
		m_pDebug.PrintW( IDebugOutput::mt_info, L"ResponseData from %s - [camera %d] Data=%s, Image=%s",
							NET_AdrToString(NetAddr).c_str(),
							nCameraNo, 
							szDataSizeBuf,
							szImgSizeBuf
						);
		
			
		try
		{
			m_ServerCameraData.SetData( nCameraNo, DataArr );
			m_ServerCameraData.SetImage( nCameraNo, ImgArr );
			m_HallAnalyzer->m_CameraData[ nCameraNo ].SetData( &DataArr[0], DataArr.size() ) ;
		}
		catch(NoSuchCameraException)
		{
			m_pDebug.PrintW( IDebugOutput::mt_error, 
				L"CMainServerImpl::OnResponseData no such camera in request when image&data arrived for [camera %d]",
				nCameraNo);
		}
	}
	m_HallAnalyzer->m_SyncCS.Unlock();
	m_pNetworkInt->TCP_CloseClient( m_nRoomID, NetAddr );
}

void CMainServerImpl::OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec )
{
	// ignore our message
	if( NET_IsOwnAddress( NetAddr ) ) return;

	std::wstring sHallComp = GetHallComputerName();
	netadr_t HallNetAddr;
	NET_StringToAdr( sHallComp.c_str(),  &HallNetAddr );

	if( !NET_CompareBaseAdr( HallNetAddr, NetAddr ) ) return; // not out room server
	WCHAR szGuid[MAX_PATH];
	StringFromGUID2( SessionID, szGuid, MAX_PATH );
	m_pDebug.PrintW( IDebugOutput::mt_info, L"Receive StartWork from %s with SessionID=%s", 
		NET_AdrToString(NetAddr).c_str(), szGuid );
	SessionServer::StartClientSession( SessionID );

	if( m_State == ss_running )
	{
		StopWork(true);
	}
	SetFilmTime( std::make_pair( nStartSec, nEndSec ) );
	StartWork(true);
}

void CMainServerImpl::OnStopWork( netadr_t NetAddr )
{
	// ignore our message
	if( NET_IsOwnAddress( NetAddr ) ) return;

	std::wstring sHallComp = GetHallComputerName();
	netadr_t HallNetAddr;
	NET_StringToAdr( sHallComp.c_str(),  &HallNetAddr );

	if( !NET_CompareBaseAdr( HallNetAddr, NetAddr ) ) return; // not out room server
	m_pDebug.PrintW( IDebugOutput::mt_info, L"Receive Stop from %s", NET_AdrToString(NetAddr).c_str() );
	StopWork(true);
}

void CMainServerImpl::OnCurrentState(
									 netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
									 __int64 nBeginSec, __int64 nEndSec,
									 const std::vector< CurrentStateType >& Devices
									 )
{
	int RoomServerState = CurrentStateType::State_Dead;
//#define PRINT_CURRENT_STATE
#ifdef PRINT_CURRENT_STATE
	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Receive CurrentState from %s", NET_AdrToString(NetAddr).c_str() );	
#endif
	for( size_t i = 0; i < Devices.size(); ++i )
	{
		const CurrentStateType& cs = Devices[i];
		std::wstring sState;
		switch(cs.State) 
		{
		case CurrentStateType::State_Dead:			sState = L"DEAD";		break;
		case CurrentStateType::State_Stopped:		sState = L"STOPPED";	break;
		case CurrentStateType::State_Working:		sState = L"WORKING";	break;
		case CurrentStateType::State_Processing:	sState = L"PROCESSING"; break;
		default: sState = L"UNKNOWN";
		}
		switch( cs.DevType )
		{
		case CurrentStateType::SubTypeServer  : 
#ifdef PRINT_CURRENT_STATE
			m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Server, state %s", sState.c_str() ); 
#endif
			RoomServerState = cs.State;
			break;
		case CurrentStateType::SubTypeCamera :  
#ifdef PRINT_CURRENT_STATE
			m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Camera %d, state %s", cs.Id, sState.c_str() ); 
#endif
			break;
		case CurrentStateType::SubTypeHall : 
#ifdef PRINT_CURRENT_STATE
			m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Hall, state %s", sState.c_str() ); 
#endif
			break;
		default:
#ifdef PRINT_CURRENT_STATE
			m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Unknown device %d, state %s", cs.Id, sState.c_str() ); 
#endif
			break;
		}
	}
	// our or not our hall server
	if( NET_IsOwnAddress( NetAddr ) || nRoomID != m_nRoomID ) return;

	std::wstring sHallComp = GetHallComputerName();
	netadr_t HallNetAddr;
	NET_StringToAdr( sHallComp.c_str(),  &HallNetAddr );

	if( !NET_CompareBaseAdr( HallNetAddr, NetAddr ) ) return; // not our room server
	WCHAR szGuid[MAX_PATH];
	StringFromGUID2( SessionID, szGuid, MAX_PATH );
	// receive CurrentState packet from our room server with the different session id
	// it means server must be restarted
	bool bRequiredNewSession = SessionServer::IsNewSessionRequired( SessionID );
	if( bRequiredNewSession )
	{
		if(	IsRemoteServerWorking(RoomServerState) )
		{
			m_pDebug.PrintW( IDebugOutput::mt_info, L"Receive CurrentState from %s with other SessionID=%s, server will be restarted",  
							NET_AdrToString(NetAddr).c_str(), szGuid );
			SessionServer::StartClientSession( SessionID );
			if( m_State == ss_running )
			{
				StopWork(true);
			}
			SetFilmTime(std::make_pair( nBeginSec, nEndSec )  );
			StartWork(true);
		}
		return;
	}
	if( !IsRemoteServerWorking( RoomServerState )  && IsLocalServerWorking( m_State) )
	{
		m_pDebug.PrintW( IDebugOutput::mt_info, L"Receive CurrentState from %s with Stop state, server will be stopped",  
			NET_AdrToString(NetAddr).c_str(), szGuid );
		StopWork(true);
	}
}

void SaveBitmap( const void* pBytes, size_t nSize)
{
	TCHAR Buffer[MAX_PATH];
	static LONG Number = 0;

#ifdef USE_JPEG
	_sntprintf(Buffer, MAX_PATH, _T("images\\CameraSnapshot%03d.jpg"), Number);
#else
	_sntprintf(Buffer, MAX_PATH, _T("images\\CameraSnapshot%03d.bmp"), Number);
#endif
	TCHAR szPathBuf[MAX_PATH];
	TCHAR szNewPathBuf[MAX_PATH];
	GetModuleFileName( GetModuleHandle(NULL), szPathBuf, MAX_PATH );
	PathRemoveFileSpec( szPathBuf );
	PathCombine( szNewPathBuf, szPathBuf, Buffer );

	TCHAR szDir[MAX_PATH];
	lstrcpyn( szDir, szNewPathBuf, MAX_PATH  );
	PathRemoveFileSpec( szDir );
	SHCreateDirectoryEx( NULL, szDir, 0 );

	FILE* f = _tfopen( szNewPathBuf, _T("wb") );
	if( !f) return;

#ifndef USE_JPEG
	BITMAPFILEHEADER Header;
	memset( &Header, 0, sizeof(Header) );
	const BITMAPINFOHEADER* pBmpHdr = (const BITMAPINFOHEADER*)pBytes;
	int nOff = ( pBmpHdr->biBitCount > 0 && pBmpHdr->biBitCount <= 8 ) ? sizeof(RGBQUAD) *( 1 << pBmpHdr->biBitCount) : 0;
	Header.bfSize = nSize + sizeof(BITMAPFILEHEADER);
	Header.bfType = MAKEWORD('B' ,'M');
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nOff;
	fwrite( &Header, sizeof(BITMAPFILEHEADER), 1, f );
	const BYTE* z = (BYTE*)pBytes;
#endif
	fwrite( pBytes, nSize, 1, f );
	fclose(f);
	InterlockedIncrement( &Number );
}

void CMainServerImpl::SendMessage( netadr_t NetAddr, const void* Msg, size_t nSize )
{
	try
	{
		m_pNetworkInt->UDP_SendMessage( m_nRoomID, NetAddr, Msg, nSize );	
	}
	catch(NetException & ex)
	{
		m_pDebug.PrintA( IDebugOutput::mt_error, "%s", ex.what() );
	}
}

void CMainServerImpl::SendReliableMessage( netadr_t NetAddr, const void* Msg, size_t nSize )
{
	try
	{
		NetAddr.port = NET_HostToNetShort( m_NetConfig.m_wTCPServerPort );
		m_pNetworkInt->TCP_SendFromClient( m_nRoomID, NetAddr, Msg, nSize );
	}
	catch(NetSysErrorException & ex)
	{
		m_pDebug.PrintA( IDebugOutput::mt_error, "%s", ex.what() );
	}
}

void CMainServerImpl::OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  )
{
	try
	{
		m_pNetworkInt->TCP_CloseClient( m_nRoomID, NetAddr );
	}
	catch(NetSysErrorException & ex)
	{
		m_pDebug.PrintA( IDebugOutput::mt_error, "%s", ex.what() );
	}
}

std::vector<int>	CMainServerImpl::GetAllCameras() const
{
	std::vector<int>		Cameras;

	for(size_t i = 0 ; i < m_LocalCameras.size(); ++i)
	{
		int nCameraNo = m_LocalCameras[i].m_nCameraNo;
		Cameras.push_back( nCameraNo );
	}
	for(size_t i = 0 ; i < m_RemoteCameras.size(); ++i)
	{
		int nCameraNo = m_RemoteCameras[i].m_nCameraNo;
		Cameras.push_back( nCameraNo );
	}
	return Cameras;
}

void	CMainServerImpl::UpdateStat()
{
	// we hold all images
	if( m_Spectators.IsReady() && 
		m_StatServer.get()		&& 
		m_ServerCameraData.IsAllImages() &&
		IsMainServer()	)
	{
		int nSpectators = m_Spectators.Get();
		if( nSpectators < 0 )
		{
			m_pDebug.PrintW( IDebugOutput::mt_warning, L"Spectator number=%d < 0, the statistics is invalid", nSpectators );
		}
		DBTIMESTAMP Time;
		SYSTEMTIME st;
		GetLocalTime( &st );
		Time.year	= st.wYear;
		Time.month	= st.wMonth;
		Time.day	= st.wDay;
		Time.hour	= st.wHour;
		Time.minute = st.wMinute;
		Time.second = st.wSecond;
		Time.fraction = 0;
		const std::vector<int>& CamNo = m_ServerCameraData.GetCameras();
		std::map<int , std::vector<BYTE> > AvailableImages;
		for( size_t i = 0; i < CamNo.size(); ++i )
		{
			const std::vector<BYTE>& Img = m_ServerCameraData.GetImage( CamNo[i] );
			AvailableImages.insert( std::make_pair( CamNo[i], Img ) );
#if (defined _DEBUG ) && (defined SAVE_DB_IMAGE)
			SaveBitmap( &Img[0], Img.size() );
#endif
		}
		m_StatServer->Statistics( m_nRoomID, Time, AvailableImages, nSpectators );
		m_ServerCameraData.Clear();
	}
}

void	CMainServerImpl::PumpState()
{
	// and local also
	if( m_StatServer.get() && IsMainServer() )
	{
	
	int nRoomID = m_nRoomID;
	int nAfter, nBefore, nPeriod;
	FilmTime ft;
	HRESULT hr;
	hr = m_StatServer->GetWorkParam( nRoomID,  nAfter, nBefore, nPeriod );
	hr= m_StatServer->GetCurrentOrNextFilmTime( nRoomID, ft );
	TimeSheduler::Settings sets;
	switch(hr)
	{
	case S_OK: 
		{
			sets.m_FilmTime = ft;
			sets.m_nAfter	= nAfter;
			sets.m_nBefore	= nBefore;
			sets.m_nPeriod	= nPeriod;
			m_TimeSheduler.ProcessNormalSettings(  m_pTimeAction.get(), sets );
			break;
		}
	case E_FAIL:
	case S_FALSE:
	default:
		{
			m_TimeSheduler.ProcessNoSetting( m_pTimeAction.get() );
		}
	}
	}
}

void	CMainServerImpl::PopThreadMessage()
{
	// dont wait
	ThreadMessageAutoEnter tme(  m_ServerMessage, false );
	while( tme.IsEntered() && !m_ServerMessage.IsEmpty(tme) )
	{
		int nID;
		const boost::any&	Data = m_ServerMessage.Front( tme, nID );
		switch( nID )
		{
		case rsp_cam_data :
			try
			{
				const CameraDataForProcessing & cd = boost::any_cast<CameraDataForProcessing>( Data ); 
				try
				{
#ifndef USE_NETWORK_FOR_TRANSITION
				if( IsMainServer() && m_HallAnalyzer.get() )
				{
						m_HallAnalyzer->m_CameraData[ cd.m_nCameraNo ].SetData( &cd.m_Data[0], cd.m_Data.size() ) ;
						m_ServerCameraData.SetData( cd.m_nCameraNo, cd.m_Data );
						m_ServerCameraData.SetImage( cd.m_nCameraNo, cd.m_Image );
				}
				else
#endif
				{
#ifdef USE_NETWORK_FOR_TRANSITION
					m_ClientCameraData.SetData( cd.m_nCameraNo, cd.m_Data );
					m_ClientCameraData.SetImage( cd.m_nCameraNo, cd.m_Image );	
#endif
					TrySendDataResponse();
				}
				}
				catch(NoSuchCameraException)
				{
					m_pDebug.PrintW( IDebugOutput::mt_error, 
						L"CMainServerImpl::PopThreadMessage - no such camera in request when image&data arrived for camera=%d",
						cd.m_nCameraNo);
				}
			}
			catch( boost::bad_any_cast )
			{
				// error!!!
				m_pDebug.PrintW( IDebugOutput::mt_error, L"Server received rsp_cam_data message with unknown content" );
			}
			break;
		case rsp_hall_spectators:
			try
			{
				const SpectatorData & sd = boost::any_cast<SpectatorData>( Data ); 
				m_Spectators.Set( sd.m_nSpectators);
				TrySendHallData(sd.m_HallData);
			}
			catch( boost::bad_any_cast )
			{
				// error!!!
				m_pDebug.PrintW( IDebugOutput::mt_error, L"Server received rsp_hall_spectators message with unknown content" );
			}
			break;
		default:
			m_pDebug.PrintW( IDebugOutput::mt_error, L"Server - unknown thread message=%d", nID );
		}
		m_ServerMessage.Pop(tme);
	}
}

void	CMainServerImpl::PushImageThreadMessage(int nCameraNo)
{
	// wait cause we lost message
	CameraThread* pThread = FindCameraThread( nCameraNo );
//	ThreadMessageAutoEnter tme(  pThread->m_CameraMessage, true );
	std::vector<BYTE> NullData;
	pThread->PushMessage( rqt_cam_data,  NullData );
}

void	CMainServerImpl::PumpImage()
{
	PopThreadMessage();
}

void	CMainServerImpl::StartProcessing()
{
	std::vector<int>	AllCameraIndex;
	netadr_t OwnAddr;
	NET_GetOwnAddress( m_NetConfig.m_wTCPServerPort, &OwnAddr );
	//split to local & remote cameras
	for(size_t i = 0 ; i < m_LocalCameras.size(); ++i)
	{
		int nCameraNo = m_LocalCameras[i].m_nCameraNo;
		AllCameraIndex.push_back( nCameraNo );
	}
	for(size_t i = 0 ; i < m_RemoteCameras.size(); ++i)
	{
		int nCameraNo = m_RemoteCameras[i].m_nCameraNo;
		AllCameraIndex.push_back( nCameraNo );
	}
	m_ServerCameraData.NewRequest( OwnAddr, AllCameraIndex );
}

void	CMainServerImpl::RefreshAlgoParam()
{
	if( !m_StatServer ) return;
	HRESULT hr;
	std::vector<BYTE> Data;
	hr = m_StatServer->GetAlgoParam( m_nRoomID, Data );
	if( hr != S_OK ) Data.clear();
	for( size_t i = 0; i < m_LocalCameras.size(); ++i )
	{
		m_LocalCameras[i].m_AlgoParam = Data;
	}
	if( IsLocalHallAnalyzer() )
	{
		m_HallConfig.m_AlgoParam = Data;
	}
}

std::wstring	CMainServerImpl::GetOwnName() const
{
	// NOTE we can use not own DNS name but IP address
	WCHAR szName[MAX_PATH];
	DWORD dwSize = MAX_PATH;
	GetComputerNameW(szName, &dwSize);
	std::wstring sName(szName);
	std::locale loc1 ( "English_US" );
	std::use_facet<std::ctype<wchar_t> > ( loc1 ).tolower ( &sName[0], & (*sName.end()) );
	return sName;
}

void	CMainServerImpl::TrySendHallData( const std::vector<BYTE>& HallData )
{
	std::set<std::wstring> Hosts;
	Hosts.insert( GetOwnName() );

	for(size_t i = 0 ; i < m_RemoteCameras.size(); ++i)
	{
		std::wstring sHostName = m_RemoteCameras[i].m_sRemoteUrl;

		std::locale loc1 ( "English_US" );
		std::use_facet<std::ctype<wchar_t> > ( loc1 ).tolower ( &sHostName[0], & (*sHostName.end()) );

		Hosts.insert( sHostName );
	}

	std::set<std::wstring>::const_iterator it = Hosts.begin();
	for( ; it != Hosts.end(); ++it)
	{
		try
		{
			netadr_t NetAddr;
			NET_StringToAdr( it->c_str(), &NetAddr );
			NetAddr.port = NET_HostToNetShort( m_NetConfig.m_wTCPServerPort );
			NetHallData Packet( HallData );
			m_pNetworkInt->TCP_SendFromClient( m_nRoomID, NetAddr, &Packet.m_Message[0], Packet.m_Message.size() );
		}
		catch ( NetException& ex) 
		{
			m_pDebug.PrintA( IDebugOutput::mt_error, ex.what() );
		}
	}
}

void CMainServerImpl::OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  )
{
	WCHAR szDataSizeBuf[100];
	StrFormatByteSizeW( Data.size(), szDataSizeBuf, 100 );

	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[NetObserver]HallData from %s - size=%s",
		NET_AdrToString(NetAddr).c_str(),
		szDataSizeBuf
		);

	for(size_t i = 0; i < m_LocalCameras.size(); ++i )
	{
//		int CameraNo = LocalIndex[i];
		CameraThread* pThread = FindCameraThread( m_LocalCameras[i].m_nCameraNo );
		pThread->PushMessage( hall_cam_data,  Data );
	}

	m_pNetworkInt->TCP_CloseClient( m_nRoomID, NetAddr );
}

////////////////////////////////////////////////////

EXPOSE_INTERFACE( CMainServerImpl, IMainServer, MAIN_SERVER_INTERFACE_0 )