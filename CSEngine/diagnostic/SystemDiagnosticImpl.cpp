//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	implemetation of ISystemDiagnostic
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 10.02.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "SystemDiagnosticImpl.h"
#include "../EngineError.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "../../CSChair/interfaceEx.h"
#include "../CSEngine.h"
#include "../syscfg_int.h"
#include "../mainserver/NetPort.h"
#include "../IPC/NetTcpLink.h"
#include "..\Timeouts.h"

#undef  DUMP_STATE
//======================================================================================//
//                              class SystemDiagnosticImpl                              //
//======================================================================================//
EXPOSE_INTERFACE( SystemDiagnosticImpl, ISystemDiagnostic, SYSTEM_DIAGNOSTIC_INTERFACE )

SystemDiagnosticImpl::SystemDiagnosticImpl() : 
	m_NetProto(this),
	m_bInMulticast(false)
{
}

SystemDiagnosticImpl::~SystemDiagnosticImpl()
{
	Uninitialize();
}
									
HRESULT	SystemDiagnosticImpl::Initialize( IDebugOutput* pDebugInt	)	
{
	m_Debug.Set( pDebugInt );
	Uninitialize();

	INetSystemConfig* pNetInt = NULL;
	DWORD dwRes = CreateEngineInterface( NET_CONFIG_INTERFACE, (void**)&pNetInt );
	if( dwRes != 0 || !pNetInt )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create INetSystemConfig" );
		return E_NOINTERFACE;
	}
	NetConfig NetCfg;
	HRESULT hr = pNetInt->GetNetConfig( NetCfg );
	pNetInt->Release();
	if( hr != S_OK )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"INetSystemConfig::GetNetConfig failed" );
		return hr;
	}

	try
	{
		m_NetLink.StartNetCommunication( NetCfg.m_wUDPServerPort, 0 );
	}
	catch (NetException& ex) 
	{
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
		return ERR_NETWORK_FAILED;
	}
	try
	{
		m_NetLink.AddToGroup( NetCfg.m_dwMultiCastGroup );
		m_bInMulticast = true;
	}
	catch (NetException& ex)
	{
		m_bInMulticast = false;
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
	}

	unsigned int dwId;

	HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, Handled_ThreadProc, this, /*CREATE_SUSPENDED*/0, &dwId  );
	if( reinterpret_cast<uintptr_t>( hThread ) != -1  )
	{
		m_WorkThread.Start( hThread );
	}
	else
	{
		m_NetLink.StopNetCommunication(  );
		m_Debug.PrintW( IDebugOutput::mt_error, L"Failed to create the diagnostic thread");
		return E_FAIL;
	}
	m_Debug.PrintW( IDebugOutput::mt_info, L"ISystemDiagnostic::Initialize succeeded" );
	return S_OK;
}

HRESULT	SystemDiagnosticImpl::GetComputerState( LPCWSTR szServerName, int nRoomID, DeviceState& st )
{
	CTime timeNow = CTime::GetCurrentTime();
	CTimeSpan timePeriod( 0, 0, 0, 5); // 5s
	st = ds_offline;
	netadr_t NetAddr;
	bool res = NET_StringToAdr( szServerName, &NetAddr );
	if( !res ) return E_FAIL;
	ServerStateArr_t::const_iterator it = std::find_if( m_ServerStates.begin(), m_ServerStates.end(), ServerAddrFunc( NetAddr, nRoomID )  );
	if( it != m_ServerStates.end() && ( timeNow - it->m_timeRecord ) < timePeriod )
	{
		st = it->m_State;
	}
	return S_OK;
}

HRESULT SystemDiagnosticImpl::GetCameraState( int nCameraNo, DeviceState& st)
{
//	st = ds_offline;
	CTime timeNow = CTime::GetCurrentTime();
	CTimeSpan timePeriod( 0, 0, 0, 5); // 5s
	st = ds_offline;
	CameraStateArr_t::const_iterator it = m_CameraStates.find(  nCameraNo  );
	if( it != m_CameraStates.end() && 
		( timeNow - it->second.first ) < timePeriod )
	{
		st = it->second.second;
	}

	return S_OK;
}

HRESULT SystemDiagnosticImpl::GetExchangeState( LPCWSTR szServerName, DeviceState& st )
{
	CTime timeNow = CTime::GetCurrentTime();
	CTimeSpan timePeriod( 0, 0, 0, 5); // 5s
	st = ds_offline;
	netadr_t NetAddr;
	bool res = NET_StringToAdr( szServerName, &NetAddr );
	if( !res ) return E_FAIL;
	ExchangeStateArr_t::const_iterator it = std::find_if( m_ExchangeStates.begin(), 
														m_ExchangeStates.end(), 
														ExchangeAddrCmpFunc( NetAddr )  );
	if( it != m_ExchangeStates.end() && ( timeNow - boost::get<1>(*it) ) < timePeriod )
	{
		st = boost::get<2>(*it);
	}
	return S_OK;
}

// this data come from utp ports

void SystemDiagnosticImpl::OnCurrentState(	netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
										  __int64 nBeginSec, __int64 nEndSec,
										  const std::vector< CurrentStateType >& Devices)
{
	NetAddr.port = 0;

	for(  size_t i = 0; i < Devices.size(); ++i )
	{
		DeviceState st = ds_offline;
		const CurrentStateType& DevSt = Devices[i]; 
		switch ( Devices[i].State )
		{
		case CurrentStateType::State_Stopped:		st = ds_stopped;	break;
		case CurrentStateType::State_Working:		st = ds_working;	break;
		case CurrentStateType::State_Processing:	st = ds_processing; break;
		case CurrentStateType::State_Dead:			
		default:
													st = ds_offline;	break;
		}

		int nId = Devices[i].Id;
		switch( Devices[i].DevType )
		{
		case CurrentStateType::SubTypeServer  :
#ifdef DUMP_STATE
			DumpServerState( NetAddr, st);
#endif
			SetServerState( NetAddr , nRoomID, st);
			break;
		case CurrentStateType::SubTypeCamera :  
#ifdef DUMP_STATE
			DumpCameraState( nId, st);
#endif
			SetCameraState( nId , st);
			break;
		case CurrentStateType::SubTypeExchange  :
#ifdef DUMP_STATE
			DumpExchangeState( NetAddr, st);
#endif
			SetExchangeState( NetAddr, st);
			break;
		default:
			break;
		}
	}
	
}

void SystemDiagnosticImpl::OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec ) 
{

}

unsigned int WINAPI SystemDiagnosticImpl::Handled_ThreadProc(void * pParam)
{
	unsigned int Res = Thread_Exception;
	__try
	{
		Res = ThreadProc(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Diagnostic thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

unsigned int WINAPI	SystemDiagnosticImpl::ThreadProc(void* pParam)
{
	SystemDiagnosticImpl* pThis = (SystemDiagnosticImpl*)pParam;

	const int c_nThreadPeriod = 50; //ms

	while ( pThis->m_WorkThread.CanThreadWork( c_nThreadPeriod ) )
	{
		try
		{
			pThis->PumpNetworkMessage();
			InterlockedExchange( & pThis->m_hrThreadStatus, S_OK );
		}
		catch( NetException& ex)
		{
			pThis->m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
			InterlockedExchange( & pThis->m_hrThreadStatus, E_FAIL );
		}
	}
	return Thread_OK;
}

void SystemDiagnosticImpl::PumpNetworkMessage()
{
	m_NetLink.LockIncomeBuffer();
	m_StateMapSync.Lock();

	NetIncomeBuffer_t& MsgBuf = m_NetLink.GetIncomeBuffer();
	NetIncomeBuffer_t::const_iterator it = MsgBuf.begin();
	for( ; it != MsgBuf.end(); ++it )
	{
		m_NetProto.PumpMessage( it->first, it->second );
	}
	MsgBuf.clear();

	m_StateMapSync.Unlock();
	m_NetLink.UnlockIncomeBuffer();
}

void	SystemDiagnosticImpl::SetCameraState(int nId, DeviceState ds )
{
	CTime timeNow = CTime::GetCurrentTime();
	m_CameraStates[ nId ] = std::make_pair( timeNow, ds );
}

void	SystemDiagnosticImpl::SetServerState( netadr_t NetAddr, int nRoomID, DeviceState ds)
{
	CTime timeNow = CTime::GetCurrentTime();	
	ServerStateArr_t::iterator it = std::find_if( m_ServerStates.begin(), m_ServerStates.end(), ServerAddrFunc( NetAddr, nRoomID ) );

	if( it != m_ServerStates.end() )
	{
		it->m_timeRecord	= timeNow;
		it->m_State			= ds;
	}
	else
	{
		m_ServerStates.push_back( ServerState(NetAddr, nRoomID  , timeNow, ds) );
#if 0
		ServerState& ss = m_ServerStates.front();
		ss.m_NetAddr	= NetAddr;
		ss.m_State		= ds;
		ss.m_timeRecord	= timeNow;
#endif
	}
}

void	SystemDiagnosticImpl::SetExchangeState(netadr_t NetAddr, DeviceState ds )
{
	CTime timeNow = CTime::GetCurrentTime();	
	ExchangeStateArr_t::iterator it = std::find_if( m_ExchangeStates.begin(),
													m_ExchangeStates.end(), 
													ExchangeAddrCmpFunc( NetAddr ) );

	if( it != m_ExchangeStates.end() )
	{
		boost::get<1>(*it)	= timeNow;
		boost::get<2>(*it)	= ds;
	}
	else
	{
		m_ExchangeStates.push_back( boost::make_tuple(NetAddr, timeNow, ds) );
	}
}

void	SystemDiagnosticImpl::Uninitialize()
{
	bool res = m_WorkThread.Stop( ThreadTimeout_SystemDiagnostic );
	if( !res )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Diagnostic thread was terminated" );
	}
	try
	{
		m_NetLink.StopNetCommunication();
	}
	catch (NetException& ex) 
	{
		m_Debug.PrintA( IDebugOutput::mt_error, ex.what() );
	}
}

void	SystemDiagnosticImpl::DumpCameraState( int nId, DeviceState ds  )
{
	std::wstring st( L"UNKNOWN" );
	switch( ds ) {
	case ds_offline: st = L"OFFLINE"; break;
	case ds_stopped: st = L"STOPPED"; break;
	case ds_working: st = L"WORK"; break;
	case ds_processing: st = L"PROCESS"; break;
	default: ;
	}
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"message from camera=%d, state %s", nId, st.c_str() );
}

void	SystemDiagnosticImpl::DumpServerState( netadr_t NetAddr, DeviceState ds )
{
	std::wstring st( L"UNKNOWN" );
	switch( ds ) {
	case ds_offline: st = L"OFFLINE"; break;
	case ds_stopped: st = L"STOPPED"; break;
	case ds_working: st = L"WORK"; break;
	case ds_processing: st = L"PROCESS"; break;
	default: ;
	}
	std::wstring sServer = NET_AdrToString( NetAddr );
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"message from server=%s, state %s", sServer.c_str(), st.c_str() );
}

void	SystemDiagnosticImpl::DumpExchangeState( netadr_t NetAddr, DeviceState ds)
{
	std::wstring st( L"UNKNOWN" );
	switch( ds ) {
	case ds_offline: st = L"OFFLINE"; break;
	case ds_stopped: st = L"DISCONNECTED"; break;
	case ds_working: st = L"WORK"; break;
	case ds_processing: st = L"WORK"; break;
	default: ;
	}
	std::wstring sServer = NET_AdrToString( NetAddr );
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"message from exchange=%s, state %s", sServer.c_str(), st.c_str() );
}

HRESULT SystemDiagnosticImpl::Report(LPCWSTR szServerName)
{
	netadr_t NetAddr;
	bool res = NET_StringToAdr( szServerName, NetSettings::c_ServerPort, &NetAddr );
	if(!res)  return E_FAIL;
	PS_NetExchangeReport pack;
	try
	{
		TCP::NetSendClient cl(NetAddr, &pack.m_Message[0], pack.m_Message.size());
		return S_OK;
	}
	catch (NetException& ex) 
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[SystemDiagnosticImpl::Report]NetException %hs", ex.what() );
		return E_FAIL;
	}
}

HRESULT SystemDiagnosticImpl::Reconnect(LPCWSTR szServerName)
{
	netadr_t NetAddr;
	bool res = NET_StringToAdr( szServerName, NetSettings::c_ServerPort, &NetAddr );
	if(!res)  return E_FAIL;
	PS_NetExchangeReconnect pack;
	try
	{
		TCP::NetSendClient cl(NetAddr, &pack.m_Message[0], pack.m_Message.size());
		return S_OK;
	}
	catch (NetException& ex) 
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"[SystemDiagnosticImpl::Reconnect]NetException %hs", ex.what() );
		return E_FAIL;
	}
}
