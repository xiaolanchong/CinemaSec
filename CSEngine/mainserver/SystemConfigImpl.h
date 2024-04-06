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
#ifndef __SYSTEM_CONFIG_IMPL_H_INCLUDED_2504203661344607__
#define __SYSTEM_CONFIG_IMPL_H_INCLUDED_2504203661344607__

#include "../main_server.h"
#include "../syscfg_int.h"
#include "../../CSChair/COMMON/Exception.h"
#include "../DBFacet/CinemaDB.h"
#include "../DBFacet/CinemaOleDB.h"

MACRO_EXCEPTION( SysCfgException, CommonException )
MACRO_EXCEPTION( SysCfgChairException,		SysCfgException )
MACRO_EXCEPTION( SysCfgBackgroundException, SysCfgException )

class NetConfigImpl : public INetSystemConfig
{
protected:
	virtual void		Release()		{ delete this; }
	virtual HRESULT		GetNetConfig	( NetConfig& NetCfg ) ;
public:
	virtual ~NetConfigImpl() {};
};

//=====================================================================================//
//                               class SystemConfigImpl                                //
//=====================================================================================//
class SystemConfigImpl : public ISystemConfig, private NetConfigImpl
{
	virtual HRESULT		GetRoomID(std::vector<int>& nRoomIDArr);
	virtual HRESULT		SetRoomID(const std::vector<int>& nRoomIDArr)	;

	virtual HRESULT		GetConnectionString(std::wstring& sConString)	;
	virtual HRESULT		SetConnectionString(const std::wstring& sConString)	;

	virtual HRESULT	LoadFromDB( bool bShowLinkWindow, bool bLoadBinData );

	virtual HRESULT	GetRemoteCamera	( int nRoomID, std::vector<RemoteCameraConfig>& RemoteCameras)	;
	virtual HRESULT	GetLocalCamera	( int nRoomID, std::vector<LocalCameraConfig>&  LocalCameras )	;
	virtual HRESULT	GetNetConfig	( int nRoomID, NetConfig& NetCfg ) ;
	virtual HRESULT	GetHallConfig	( int nRoomID, HallConfig& HallCfg, HallProcessConfig& HallProcessCfg, bool& bLocal );
	virtual void	SetDebugInterface( IDebugOutput* pDebug );
	
	virtual void		Release() { delete this; }

	// data
	std::map<int, std::vector< LocalCameraConfig > >	m_LocalCameraConfig;
	std::map<int, std::vector< RemoteCameraConfig > >	m_RemoteCameraConfig;
	std::map<int, HallConfig	>						m_HallConfig;
	std::map<int, HallProcessConfig >					m_HallProcessConfig;

	NetConfig											m_NetCfg;
	std::wstring										m_LocalHost;

	bool									m_bAllDataWereLoaded;
	MyDebugOutputImpl						m_Debug;
	CCinemaOleDB							m_db;

	void	CreateRoomDB(CCinemaDB& db, int nRoomID, bool bLoadBinData);
//public:
	void	Preload();
	BOOL	IsNetPresent();

	bool	GetNetInterface( LPCWSTR szHost, std::vector<DWORD32> & Ints);
	bool	IsEqualComputers( LPCWSTR szLeft, LPCWSTR szRight );
public:
	SystemConfigImpl();
	virtual ~SystemConfigImpl();
};

#endif //__SYSTEM_CONFIG_IMPL_H_INCLUDED_2504203661344607__