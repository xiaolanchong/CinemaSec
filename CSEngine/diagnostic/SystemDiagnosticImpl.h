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
#ifndef _SYSTEM_DIAGNOSTIC_IMPL_2811445081271046_
#define _SYSTEM_DIAGNOSTIC_IMPL_2811445081271046_

#include "ISystemDiagnostic.h"
#include "../DBFacet/DBRows.h"
#include "../IPC/NetLink.h"
#include "../mainserver/NetProto.h"
#include "../mainserver/ThreadError.h"
#include <boost/tuple/tuple.hpp>
//======================================================================================//
//                              class SystemDiagnosticImpl                              //
//======================================================================================//

struct ServerState
{
	netadr_t						m_NetAddr;
	int								m_nRoomID;
	CTime							m_timeRecord;
	ISystemDiagnostic::DeviceState	m_State;

	ServerState( netadr_t _na, int nRoomID, CTime timeRecord, ISystemDiagnostic::DeviceState	_State ):
		m_NetAddr(_na), m_nRoomID( nRoomID ), m_timeRecord(timeRecord), m_State(_State)
		{}
};

struct ServerAddrFunc
{
	netadr_t	NetAddr;
	int			nRoomID;

	bool operator () ( const ServerState& b)
	{
		return NET_CompareBaseAdr( NetAddr, b.m_NetAddr ) && b.m_nRoomID == nRoomID;
	}

	ServerAddrFunc( netadr_t na, int _nRoomID ) : NetAddr(na), nRoomID(_nRoomID){}
};

typedef std::vector<ServerState>											ServerStateArr_t;
typedef std::map<int, std::pair<CTime, ISystemDiagnostic::DeviceState> >	CameraStateArr_t;

typedef boost::tuple< netadr_t, CTime, ISystemDiagnostic::DeviceState>		ExchangeState_t;
typedef std::vector<ExchangeState_t>										ExchangeStateArr_t;

struct ExchangeAddrCmpFunc
{
	netadr_t	NetAddr;

	bool operator () ( const ExchangeState_t& b)
	{
		return NET_CompareBaseAdr( NetAddr, boost::get<0>(b) );
	}

	ExchangeAddrCmpFunc( netadr_t na ) : NetAddr(na){}
};

class SystemDiagnosticImpl :	public	ISystemDiagnostic,
								private	INetProtoCallback
{
	// for dynamic creation
public:
	SystemDiagnosticImpl();
private:
	virtual ~SystemDiagnosticImpl();

	// ISystemDiagnostic
	virtual void	Release() { delete this;}										;
	virtual HRESULT	Initialize(IDebugOutput* pDebugInt)	;
	virtual HRESULT	GetComputerState(LPCWSTR szServerName, int nRoomID, DeviceState& st )				;
	virtual HRESULT GetCameraState( int nCameraNo, DeviceState& st)		;

	virtual HRESULT GetExchangeState( LPCWSTR szServerName, DeviceState& st );
	virtual	HRESULT Report(LPCWSTR szServerName);
	virtual	HRESULT Reconnect(LPCWSTR szServerName);

	// ====== INetProtoCallback, trivial ====== 
	virtual void OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ){};
	virtual void OnResponseData( netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data ) {};
	virtual void OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec );
	virtual void OnStopWork( netadr_t NetAddr ){};
	virtual void OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  ){}
//	virtual void OnExchangeReconnect(netadr_t NetAddr) {};
//	virtual void OnExchangeReport(netadr_t NetAddr) {};
	// ====== implemented =====
	// for UDP datagrams ( ping, start, state )
	virtual void SendMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) {};
	// for TCP packets ( data, requests )
	virtual void SendReliableMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) {};
	virtual void OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  ) {};
	// we use only this function
	virtual void OnCurrentState(netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
								__int64 nBeginSec, __int64 nEndSec,
								const std::vector< CurrentStateType >& Devices);

	// own
	static unsigned int WINAPI	ThreadProc(void* pParam);
	static unsigned int WINAPI	Handled_ThreadProc(void* pParam);
	void	PumpNetworkMessage();

	// data
	NetLink		m_NetLink;
	NetProto	m_NetProto;
	ThreadSync	m_WorkThread;
	bool		m_bInMulticast;

	SyncCriticalSection	m_StateMapSync;

	ServerStateArr_t									m_ServerStates;
	CameraStateArr_t									m_CameraStates;
	ExchangeStateArr_t									m_ExchangeStates;

	HRESULT												m_hrThreadStatus;

	void	SetCameraState( int nId, DeviceState ds );
	void	SetServerState( netadr_t, int nRoomID, DeviceState ds);
	void	SetExchangeState( netadr_t NetAddr, DeviceState ds);
	void	Uninitialize();

	MyDebugOutputImpl					m_Debug;

	enum
	{
		Thread_OK			= 0,
		Thread_Exception	= 1
	};

	void	DumpCameraState( int nId, DeviceState ds  );
	void	DumpServerState( netadr_t NetAddr, DeviceState ds );
	void	DumpExchangeState(netadr_t NetAddr, DeviceState ds );
};

#endif // _SYSTEM_DIAGNOSTIC_IMPL_2811445081271046_