#ifndef __SYSCFG_INT_H__
#define __SYSCFG_INT_H__

#include "../csalgo/algorithm/base_chair.h"

#define		SYSTEM_DB_CONFIG_INTERFACE_0		L"{61E2DB95-6812-402E-8DCE-4F0298265C1F}"
#define		NET_CONFIG_INTERFACE				L"{BFCAA42A-F12A-4C41-AE3A-4DEBA930F738}"

struct RemoteCameraConfig
{
	int 				m_nCameraNo;
	std::wstring		m_sRemoteUrl;

	RemoteCameraConfig() : m_nCameraNo(-1){}
};

struct LocalCameraConfig
{
	int								m_nCameraNo;
	Arr2f							m_Background;
	std::wstring					m_sVideoSourceUrl;
	std::vector<BaseChair>			m_Chairs;	
	std::vector<BYTE>				m_AlgoParam;

	LocalCameraConfig() : m_nCameraNo(-1){}
};

struct HallConfig
{
	// if local, array will be filled
	std::vector<BaseChair>			m_Chairs;
	std::wstring					m_sHallUrl;
	// if local, array will be filled
	std::vector<BYTE>				m_AlgoParam;

	bool							m_bLocal;
	bool IsLocal() const { return m_bLocal;}
};

struct NetConfig
{
	WORD		m_wTCPServerPort;
	WORD		m_wUDPServerPort;
	DWORD		m_dwMultiCastGroup;
};

// only for local hall
struct HallProcessConfig
{
	// time for processing hall analyzer
	DWORD		m_nProcessingPeriod;
	// time for requesting period
	DWORD		m_nRequestDataPeriod;
	// how many times we'll request data
	BYTE	m_nRequestDataNumber;
};

struct INetSystemConfig
{
	virtual HRESULT	GetNetConfig	( NetConfig& NetCfg ) = 0;
	virtual void	Release()		= 0;
};

struct ISystemConfig
{
	virtual HRESULT		GetRoomID(std::vector<int>& nRoomIDArr)	= 0;
	virtual HRESULT		SetRoomID(const std::vector<int>& nRoomIDArr)	= 0;

	virtual HRESULT		GetConnectionString(std::wstring& sConString)	= 0;
	virtual HRESULT		SetConnectionString(const std::wstring& sConString)	= 0;

	virtual HRESULT	LoadFromDB( bool bShowLinkWindow, bool bLoadBinData ) = 0;

	virtual HRESULT	GetRemoteCamera	( int nRoomID, std::vector<RemoteCameraConfig>& RemoteCameras)	= 0;
	virtual HRESULT	GetLocalCamera	( int nRoomID, std::vector<LocalCameraConfig>&  LocalCameras )	= 0;
	
	virtual HRESULT	GetNetConfig	( int nRoomID, NetConfig& NetCfg ) = 0;
	virtual HRESULT	GetHallConfig	( int nRoomID, HallConfig& HallCfg, HallProcessConfig& HallProcessCfg, bool& bLocal ) = 0;
	virtual void	SetDebugInterface( IDebugOutput* pDebug ) = 0;
	virtual void	Release()		= 0;
};

inline void	DestroySystemConfigInt(ISystemConfig* p)
{
	p->Release();
}

#endif //__SYSCFG_INT_H__