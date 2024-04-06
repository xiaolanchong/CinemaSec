//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   01.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "SystemConfigImpl.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "../../CSChair/chair_int.h"
#include "../../CSChair/interfaceEx.h"
#include "../DBFacet/MemoryBackground.h"
#include "../EngineError.h"
#include "../IPC/NetAddr.h"
#include "NetPort.h"

//last header
#include "../../CSChair/public/memleak.h"

//#define SUPER_THROW

#define ERR_OK			S_OK
#define ERR_ERROR		E_FAIL
#define ERR_NOTIMPL		E_NOTIMPL
#define ERR_INVALIDARG	E_INVALIDARG

#define szSubKey				RegSettings::c_szCinemaSecSubkey
#define szRoomValue				_T("RoomID")
#define szCinemaValue			_T("CinemaID")
#define szConnectionStringValue	_T("DBConnectionString")

HRESULT	NetConfigImpl::GetNetConfig	( NetConfig& NetCfg )
{
	const WORD	BasePortNumber = 13420;
	NetCfg.m_wTCPServerPort		= BasePortNumber;
	NetCfg.m_wUDPServerPort		= BasePortNumber;
	NetCfg.m_dwMultiCastGroup = inet_addr( "225.6.7.8");
	return S_OK;
}

EXPOSE_INTERFACE( NetConfigImpl, INetSystemConfig, NET_CONFIG_INTERFACE );

//=====================================================================================//
//                               class SystemConfigImpl                                //
//=====================================================================================//

EXPOSE_INTERFACE( SystemConfigImpl, ISystemConfig, SYSTEM_DB_CONFIG_INTERFACE_0 );

SystemConfigImpl::SystemConfigImpl():
	m_bAllDataWereLoaded(false)
{
	// FIXME : TO_DB_CONFIG

}

SystemConfigImpl::~SystemConfigImpl()
{
}

void	SystemConfigImpl::SetDebugInterface( IDebugOutput* pDebug )
{
	m_Debug.Set( pDebug );
}

HRESULT		SystemConfigImpl::GetRoomID(std::vector<int>& nRoomIDArr)
{
	const int Max_Rooms = 128;
	INT32	RoomIDBuffer[Max_Rooms];
	DWORD	dwType = REG_BINARY;
	DWORD	dwSize = sizeof(RoomIDBuffer);
	DWORD res = SHGetValue( HKEY_LOCAL_MACHINE, szSubKey, szRoomValue, &dwType, &RoomIDBuffer, &dwSize );
	if( res != ERROR_SUCCESS || dwSize < sizeof(INT32))
	{
		return ERR_ROOMID_NOT_SET;
	}	
	nRoomIDArr.assign( RoomIDBuffer, RoomIDBuffer +  dwSize/ sizeof(INT32) );
	return S_OK;
}

HRESULT		SystemConfigImpl::SetRoomID(const std::vector<int>& nRoomIDArr)
{
	return E_NOTIMPL;
}

HRESULT		SystemConfigImpl::GetConnectionString(std::wstring& sConString)
{
	TCHAR szCS[1024];
	DWORD	dwData = sizeof(szCS),
			dwType = REG_SZ;
	DWORD res = SHGetValueW(HKEY_LOCAL_MACHINE, szSubKey, szConnectionStringValue, &dwType, szCS, &dwData);
	if( (res == ERROR_SUCCESS)  )
	{
		sConString = szCS;
		return S_OK ;
	}
	else	
		return ERR_DBCONNECTIONSTRING_NOT_SET;
}

HRESULT		SystemConfigImpl::SetConnectionString(const std::wstring& sConString)
{
	DWORD	dwSize = DWORD(sConString.size() * sizeof(wchar_t)) ,
		dwType = REG_SZ;
	DWORD res = SHSetValueW( 
		HKEY_LOCAL_MACHINE, szSubKey, szConnectionStringValue, 
		REG_SZ, sConString.c_str(), dwSize );
	return res == ERROR_SUCCESS ? S_OK : E_FAIL;
}

HRESULT	SystemConfigImpl::LoadFromDB( bool bShowLinkWindow, bool bLoadBinData ) 	
try
{
	Preload();
	// start try
	std::wstring sCS;
	GetConnectionString( sCS );
	m_db.SetSource(sCS.c_str() , false);
	try
	{
		m_db.Open();	
	}
	catch(DBConnectionException&)
	{
		if( !bShowLinkWindow ) throw;
		m_db.SetSource(0, true);
		m_db.Open();
		std::tstring z = m_db.GetConnectionString();
		SetConnectionString( z );
	}
	m_db.ReadData();

	std::vector<int> RoomIDArr;
	GetRoomID(RoomIDArr);
	for(size_t i = 0 ; i < RoomIDArr.size(); ++i)
		CreateRoomDB( m_db, RoomIDArr[i], bLoadBinData );
	m_bAllDataWereLoaded = true;
	// end try
	return S_OK;
}
catch( DataBaseException )
{
	return ERR_DB_CONNECTION_FAILED;
}
catch(SysCfgChairException)
{
	return ERR_CHAIR_ERROR;
}
catch(SysCfgBackgroundException)
{
	return ERR_BACKGROUND_ERROR;
}
catch(SysCfgException )
{
	return ERR_SYSTEM_CONFIGURATION;
}
catch(CommonException )
{
	return ERR_COMMON_EXCEPTION;
};

HRESULT	SystemConfigImpl::GetRemoteCamera	( int nRoomID, std::vector<RemoteCameraConfig>& RemoteCameras)
{
	if( !m_bAllDataWereLoaded )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"SystemConfigImpl::GetRemoteCamera - invalid configuration" );
		return ERR_SYSTEM_CONFIGURATION_INVALID;
	}
	std::map<int, std::vector< RemoteCameraConfig > >::const_iterator it =
		m_RemoteCameraConfig.find( nRoomID );
	if( it == m_RemoteCameraConfig.end() )
	{
		// no remote cameras for our config
		RemoteCameras.clear();
		return S_OK;
	}
	RemoteCameras = it->second;
	return S_OK;
}

HRESULT	SystemConfigImpl::GetLocalCamera	( int nRoomID, std::vector<LocalCameraConfig>&  LocalCameras )
{
	if( !m_bAllDataWereLoaded )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"SystemConfigImpl::GetLocalCamera - invalid configuration" );
		return ERR_SYSTEM_CONFIGURATION_INVALID;
	}
	std::map<int, std::vector< LocalCameraConfig > >::const_iterator it =
		m_LocalCameraConfig.find( nRoomID );
	if( it == m_LocalCameraConfig.end() )
	{
		// no local cameras for our config
		LocalCameras.clear();
		return S_OK;
	}
	LocalCameras = it->second;
	return S_OK;
}

HRESULT	SystemConfigImpl::GetNetConfig	( int nRoomID, NetConfig& NetCfg )
{
	if( !m_bAllDataWereLoaded )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"SystemConfigImpl::GetNetConfig - invalid configuration" );
		return ERR_SYSTEM_CONFIGURATION_INVALID;
	}
	return NetConfigImpl::GetNetConfig( NetCfg );
}

HRESULT	SystemConfigImpl::GetHallConfig	( int nRoomID, HallConfig& HallCfg, HallProcessConfig& HallProcessCfg, bool& bLocal )
{
	if( !m_bAllDataWereLoaded )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"SystemConfigImpl::GetHallConfig - invalid configuration" );
		return ERR_SYSTEM_CONFIGURATION_INVALID;
	}
	std::map<int, HallConfig >::const_iterator it =
		m_HallConfig.find( nRoomID );
	if( it == m_HallConfig.end() )
	{
		return E_INVALIDARG;
	}
	HallCfg = it->second;
	HallProcessCfg.m_nProcessingPeriod	= 10000;
	HallProcessCfg.m_nRequestDataNumber	= 6;
	HallProcessCfg.m_nRequestDataPeriod = 5000;
	bLocal = ( IsEqualComputers ( HallCfg.m_sHallUrl.c_str() , m_LocalHost.c_str() ) );
	HallCfg.m_bLocal = bLocal;
	return S_OK;
}

void	LoadChairs( MyDebugOutputImpl& dbg,  int nRoomID, CCinemaDB& db, std::vector<BaseChair>& ChairData )
{
	std::vector<BYTE> binData;
	bool res;
	res = db.GetTableRoomFacet().GetChairs( nRoomID, binData );
	if(!res)
	{
		dbg.PrintW( IDebugOutput::mt_error, L"Failed to load chair data from db for room=%d", nRoomID );
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgChairException("No chair data for room");
	}
	if( binData.empty() )
	{
		dbg.PrintW( IDebugOutput::mt_warning, L"[WARNING] chair data from db for room=%d is empty", nRoomID );
	}
	IChairSerializer* pInt;
	DWORD dwRes = CreateInterface( CHAIR_SERIALIZER_INTERFACE_0, (void**)&pInt );
	if(dwRes != S_OK)
	{
		dbg.PrintW( IDebugOutput::mt_error, L"No serializer interface" );
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgException("No chair serialize interface");
	}
	IChairSerializer::PositionMap_t PosMapNotRequiredHere;
	dwRes = pInt->LoadGaugeHallFromBinaryData( &binData[0], binData.size(), ChairData, PosMapNotRequiredHere );
	pInt->Release();
	if(dwRes != S_OK)
	{
		dbg.PrintW( IDebugOutput::mt_error, L"Failed to load chairs for room=%d", nRoomID );
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgChairException("Failed to load chair data");
	}
}

void	LoadBackground(MyDebugOutputImpl& dbg, int nCameraID, CCinemaDB& db, Arr2f& Background)
{
	std::vector<BYTE> Arr;
	bool res = db.GetTableCameraFacet().GetBackground( nCameraID, Arr );
	if( !res) 
	{
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgException("Error occured while loading background"); 
	}
	try
	{
		if( Arr.empty() )
			dbg.PrintW( IDebugOutput::mt_warning, L"[WARNING] background for camera=%d is empty", nCameraID );
		MemoryBackground::MemoryLoadFloatImage( &Arr[0], Arr.size(), Background, INVERT_AXIS_Y );
	}
	catch(std::exception&)
	{
		dbg.PrintW( IDebugOutput::mt_error, L"Failed to load background for camera =%d", nCameraID );
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgBackgroundException("Error occured while loading background");
	}
}

void	SystemConfigImpl::CreateRoomDB( CCinemaDB& db, int nRoomID, bool bLoadBinData)
{
	std::set<RoomRow>::const_iterator itR = 
		db.GetTableRoomFacet(). GetRooms().find( RoomRow(nRoomID) );
	if( itR == db.GetTableRoomFacet().GetRooms().end())
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"No such requested room=%d", nRoomID );
#ifdef SUPER_THROW
		throw 0;
#endif
		throw SysCfgException("No such room");
	}
	std::tstring HallIP = itR->m_sIP;

	bool bLocalHall = ( IsEqualComputers ( m_LocalHost.c_str() , HallIP.c_str() )  );

	m_HallConfig[nRoomID].m_sHallUrl = HallIP;
	if( bLocalHall && bLoadBinData)
	{
		LoadChairs( m_Debug, nRoomID, db, m_HallConfig[nRoomID].m_Chairs );
	}

	std::set<int> RoomSet = db.GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
	std::set<int>::const_iterator it = RoomSet.begin();
	std::vector<BaseChair> ChairData;
	bool bChairsLoaded = bLocalHall;
	if( bLocalHall)
	{
		ChairData = m_HallConfig[nRoomID].m_Chairs;
	}
	for(; it != RoomSet.end();++it  )
	{
		std::set<CameraRow>::const_iterator itCam = db.GetTableCameraFacet(). GetCameras().find( CameraRow(*it) );
		if(itCam == db.GetTableCameraFacet().GetCameras().end())
		{
			m_Debug.PrintW( IDebugOutput::mt_error, L"Internal error  - no such camera in the configuration" );
#ifdef SUPER_THROW
			throw 0;
#endif
			throw SysCfgException("Internal error");
		}
		std::tstring sCamerIP = itCam->m_sIP;
		bool bEqual = ( IsEqualComputers ( m_LocalHost.c_str() , sCamerIP.c_str() ) );
		if(bEqual)
		{
			if( !bChairsLoaded && bLoadBinData )
			{
				LoadChairs( m_Debug, nRoomID, db, ChairData );
				bChairsLoaded = true;
			}
			LocalCameraConfig cc;
			cc.m_sVideoSourceUrl	= itCam->m_sGUID;
			cc.m_nCameraNo		= itCam->m_nID;
			cc.m_Chairs			= ChairData;
			if( bLoadBinData )
				LoadBackground( m_Debug, itCam->m_nID, db, cc.m_Background );
			m_LocalCameraConfig[nRoomID].push_back(cc);
		}
		else
		{
			RemoteCameraConfig cc;
			cc.m_sRemoteUrl	= itCam->m_sIP;
			cc.m_nCameraNo	= itCam->m_nID;
			m_RemoteCameraConfig[nRoomID].push_back(cc);
		}
	}
}

void SystemConfigImpl::Preload(  )
{
	m_bAllDataWereLoaded = false;
	m_LocalCameraConfig.clear();
	m_RemoteCameraConfig.clear();


	char Buf[ MAX_PATH ];
	int res = gethostname( Buf, MAX_PATH );
	if(res == SOCKET_ERROR)
	{
		m_LocalHost.clear();
		return;
	}
	else
	{
		USES_CONVERSION;
		LPCWSTR szBufW = A2CW( Buf );
		m_LocalHost = szBufW ? szBufW : L"";
	}
}

BOOL SystemConfigImpl::IsNetPresent()
{
	struct in_addr BIN_IPV4_ADDR_LOOPBACK = {127, 0, 0, 1};
	struct in6_addr BIN_IPV6_ADDR_LOOPBACK =   {   0x0, 0x0,
		0x0, 0x0,
		0x0, 0x0,
		0x0, 0x0,
		0x0, 0x0,
		0x0, 0x0,
		0x0, 0x0,
		0x0, 0x1 };
	#define   MAX_LOCAL_NAME_LEN               64


	BOOL bFoundLocalAddr = FALSE;
	char szAddrASCII[MAX_LOCAL_NAME_LEN];
	ADDRINFO AddrHints, *pAI, *pAddrInfo;

	//
	// Get the local host's name in ASCII text.
	//
	if(gethostname(szAddrASCII, MAX_LOCAL_NAME_LEN - 1))
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Error getting local host name, error = %d", WSAGetLastError());
		return FALSE;
	}

	//
	// To obtain a list of all the local
	// addresses, resolve the local name with getaddrinfo for all
	// protocol families.
	//

	memset(&AddrHints, 0, sizeof(AddrHints));
	AddrHints.ai_family = PF_UNSPEC;
	AddrHints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(szAddrASCII, "10", &AddrHints, &pAddrInfo))
	{
		m_Debug.PrintW(IDebugOutput::mt_error, L"getaddrinfo(%hs) error %d", szAddrASCII, WSAGetLastError());
		return FALSE;
	}

	//
	// Search the addresses returned.
	// If any of them match the loopback address, then
	// are not connected to an outside network.
	//
	// Note: This will not tell you how many networks you
	// are connected to.  If one or more networks are present,
	// then the loopback addresses will not be included in the
	// list returned from getaddrinfo.
	//

	bFoundLocalAddr = TRUE;
	for(pAI = pAddrInfo; pAI != NULL && bFoundLocalAddr; pAI = pAI->ai_next)
	{
		if(pAI->ai_family == PF_INET)
		{
			if(memcmp(&(((SOCKADDR_IN *)(pAI->ai_addr))->sin_addr), &BIN_IPV4_ADDR_LOOPBACK, sizeof(BIN_IPV4_ADDR_LOOPBACK)) == 0)
				bFoundLocalAddr = FALSE;
		}
		else if(pAI->ai_family == PF_INET6)
		{
			if(memcmp(&(((SOCKADDR_IN6 *)(pAI->ai_addr))->sin6_addr), &BIN_IPV6_ADDR_LOOPBACK, sizeof(BIN_IPV6_ADDR_LOOPBACK)) == 0)
				bFoundLocalAddr = FALSE;  
		}
	}

	freeaddrinfo(pAddrInfo);

	return bFoundLocalAddr;
}

bool	SystemConfigImpl::GetNetInterface( LPCWSTR szHost, std::vector<DWORD32> & Ints )
{
//	char szAddrASCII[MAX_LOCAL_NAME_LEN];
	ADDRINFO AddrHints, *pAI, *pAddrInfo;

	Ints.clear();
#if 0
	if(gethostname(szAddrASCII, MAX_LOCAL_NAME_LEN - 1))
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Error getting local host name, error = %d", WSAGetLastError());
		return false;
	}
#endif
	USES_CONVERSION;
	const char* szAddrASCII = W2CA( szHost );

	memset(&AddrHints, 0, sizeof(AddrHints));
	AddrHints.ai_family = PF_INET;
	AddrHints.ai_flags	= AI_PASSIVE;

	if(getaddrinfo(szAddrASCII, "10", &AddrHints, &pAddrInfo))
	{
		m_Debug.PrintW(IDebugOutput::mt_error, L"getaddrinfo(%hs) error %d", szAddrASCII, WSAGetLastError());
		return false;
	}

	for(pAI = pAddrInfo; pAI != NULL ; pAI = pAI->ai_next)
	{
		if(pAI->ai_family == PF_INET)
		{
			u_long SockAddr = ((SOCKADDR_IN *)(pAI->ai_addr))->sin_addr.S_un.S_addr;
			Ints.push_back( SockAddr );
#if 0
			netadr_t na;
			SockadrToNetadr( pAI->ai_addr, &na );
			m_Debug.PrintW(IDebugOutput::mt_info, L"getaddrinfo(%hs)", NET_AdrToStringA(na).c_str());
#endif
		}
	}

	freeaddrinfo(pAddrInfo);
	return true;
}

bool	SystemConfigImpl::IsEqualComputers( LPCWSTR szLeft, LPCWSTR szRight )
{
#if 0
	netadr_t a, b;
	NET_StringToAdr( szLeft, &a );
	NET_StringToAdr( szRight, &b );
	return NET_CompareBaseAdr( a, b );
#endif
	bool res;
	std::vector<DWORD32> LeftInt, RightInt;
	res = GetNetInterface( szLeft, LeftInt );
	if(!res) return false;
	res = GetNetInterface( szRight, RightInt);
	if(!res) return false;
	std::sort( LeftInt.begin(), LeftInt.end() );
	std::sort( RightInt.begin(), RightInt.end() );
	std::unique(LeftInt.begin(), LeftInt.end());
	std::unique(RightInt.begin(), RightInt.end());
	std::vector<DWORD32> Intersection;
	std::set_intersection( LeftInt.begin(), LeftInt.end(), RightInt.begin(), RightInt.end(), 
		std::back_inserter(Intersection ) );

#if 0 // debug
	union
	{
		DWORD32 dw;
		BYTE	ip[4];
	} sa;
	for( size_t i =0; i < LeftInt.size(); ++i )
	{
		sa.dw = LeftInt[i];
		m_Debug.PrintW(IDebugOutput::mt_info, L"%d.%d.%d.%d", sa.ip[0], sa.ip[1], sa.ip[2], sa.ip[3] );
	}

	m_Debug.PrintW(IDebugOutput::mt_info, L"Right:");
	for( size_t i =0; i < RightInt.size(); ++i )
	{
		sa.dw = RightInt[i];
		m_Debug.PrintW(IDebugOutput::mt_info, L"%d.%d.%d.%d", sa.ip[0], sa.ip[1], sa.ip[2], sa.ip[3] );
	}

	m_Debug.PrintW(IDebugOutput::mt_info, L"Intersection:");
	for( size_t i =0; i < Intersection.size(); ++i )
	{
		sa.dw = Intersection[i];
		m_Debug.PrintW(IDebugOutput::mt_info, L"%d.%d.%d.%d", sa.ip[0], sa.ip[1], sa.ip[2], sa.ip[3] );
	}
#endif
	return !Intersection.empty();
}
