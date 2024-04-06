#ifndef __MAIN_SERVER_IMPL_H__
#define __MAIN_SERVER_IMPL_H__

#include "../Main_server.h"
#include "../../CSChair/interfaceEx.h"
#include "CameraServer.h"
#include "HallServer.h"
#include "../ipc/NetLink.h"
#include "../ipc/NetTcpLink.h"
#include "NetProto.h"
#include "IStatisticsServer.h"
#include "TimeSheduler.h"
#include "BackupServer.h"
#include "SessionServer.h"
#include "CameraDataBuffer.h"

#define USE_NETWORK_LAYER

class	NetProto;

//! \brief room server thread class implementation
//! \version 1.1
//! \date 10-26-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!		\li 2005-10-26	���-�� �������� ���������� ����� ������� ���������
class CMainServerImpl : public	IMainServer, 
						public	INetProtoCallback,
						public	CameraServer,
						public	HallServer,
						protected	SessionServer
{
	friend class CameraThread;
	friend struct HallThread;

	friend class  StaStopped;
	friend class  StaWorked;
	friend class  StaProcessed;
	friend class  StaAction;

	//! ��������� �������
	enum ServerState
	{
		//! �����
		ss_begin,
		//! ���������������
		ss_initialized,
		//! �������
		ss_running,
		//! ����������
		ss_stopped
	};

	//! ���������, ServerState 
	LONG	m_State;

	//! ����������� ������� ������ �� ����� (������� �����)
	virtual void OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ) ;
	//! ����������� ������ �� ����� (������� �����)
	virtual void OnResponseData(netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data );
	//! ���������� ������ ������ (������� �����)
	virtual void OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec );
	//! ���������� ��������� ������ (������� �����)
	virtual void OnStopWork( netadr_t NetAddr );
	virtual void OnCurrentState(netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
								__int64 nBeginSec, __int64 nEndSec,
								const std::vector< CurrentStateType >& Devices);

	virtual void OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  );

	virtual void SendReliableMessage( netadr_t NetAddr, const void* Msg, size_t nSize );
	virtual void OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  );
	virtual void SendMessage( netadr_t NetAddr, const void* Msg, size_t nSize );

public:
	CMainServerImpl();
	virtual ~CMainServerImpl();


	//	DebugMode

	//! ���������� ���������� ���������
	//! \param pDebugInt ���������� ���������
	//! \return 0 - �����, ����� - ������
	virtual DWORD		SetDebugInterface(IDebugOutput* pDebugInt) ;
	//! ���������� ���� ��� ��������� ����������� ������ (�������)
	//! \param nCameraNo ������������� ������
	//! \param hWnd ����
	//! \return S_OK - �����, ����� - ������
	virtual HRESULT		SetCameraWindow(int nCameraNo, HWND hWnd) ;

	//	ControlMode
	//! ������������� ������
	//! \param pFactory ������� ������������
	//! \param pNetInt ������� ���������
	//! \return 0 - �����, ����� - ������
	virtual DWORD	Initialize(IGrabberFactory* pFactory, INetworkLayer* pNetInt )	;
	//! ���������� ������
	//! \param bForce �������������
	//! \return 0 - �����, ����� - ������
	virtual DWORD	Start	(bool bForce)		;	
	//! ���������� ������
	//! \param bForce �������������
	//! \return 0 - �����, ����� - ������
	virtual DWORD	Stop	(bool bForce)		;	

	//! �������� ��������� ������, ��� �������
	//! \param ZoneNo ������������� ������
	//! \return ���������
	virtual ICameraAnalyzer*	GetCameraAnalyzer(int ZoneNo)	;
	//! �������� ��������� ����������� ����, ��� �������
	//! \return ��������
	virtual IHallAnalyzer*		GetHallAnalyzer()				;

	virtual void	Release()	;

	//! �������������
	//! \return 0 - �����, ����� - ������
	virtual DWORD	InitializeWork()	;

	//! �������� ������������ �������
	//! \return 0 - �����, ����� - ������
	DWORD	GetDBConfig();

	//! ���������� �������
	//! \param bForce ������������� ��� �� ����������
	//! \return 0 - �����, ����� - ������
	DWORD	StartWork(bool bForce);
	DWORD	StopWork(bool bForce);
private:
	//! ������������� ��������������� ����
	int									m_nRoomID;
	//! ������ ��������� �����
	std::vector<RemoteCameraConfig>		m_RemoteCameras;
	//! ������������ �����������
	HallConfig							m_HallConfig;
	//! ������� ������������, �����, ����
	NetConfig							m_NetConfig;

	//! ������������� ������
	ThreadSync							m_MainThread;
	//! ������������� ������ � ����������� 
	SyncCriticalSection					m_AnalyzerCS;

	//! for network
	CameraDataBuffer					m_ServerCameraData;
	//! for threads
	CameraDataBuffer					m_ClientCameraData;

	//! ������ ���� ����� ����
	std::vector<int>					GetAllCameras() const;
	//! ��������� ����������, �������� ���������
	boost::shared_ptr<IStatisticsServer>	m_StatServer;
	//! ���������, � ������������ �� ����������
	std::auto_ptr<TimeAction>				m_pTimeAction;
	//! ������ ������� ����������
	TimeSheduler							m_TimeSheduler;
	//! ���������� ���������� ��� ��������� �������������, \sa StartWork
	bool									m_bUseTimeTable;
	//! ������� ���������
	INetworkLayer*						m_pNetworkInt;
	//! ������� ��� ������� �������� ���������
	NetProto							m_NetProtocol;

	//! �-� ������
	//! \param pParam �������� ��� ������
	//! \return 0 - �����, ����� ������
	static unsigned int WINAPI ServerThreadProc(void * pParam);
	//! ���������� try-catch �-� ������, ������� ServerThreadProc
	//! \param pParam �������� ��� ������
	//! \return 0 - �����, ����� ������
	static unsigned int WINAPI Handled_ServerThreadProc(void * pParam);

	//! ���������� ������
	//! \return 0 - �����, ����� ������
	DWORD	StopServer();
	//! ���������� ������
	//! \return 0 - �����, ����� ������
	DWORD	StartServer();

	//! ��������� ������ �� ���������� � �������
	//! \param Np ������� ��� �������� ��������
	//! \param Host2Cameras ������� ��� �������-������ ��� �����
	void	SendCameraRequest(NetProto& Np, const std::map < std::wstring, std::vector<int> >& Host2Cameras);

	//! ������� ����� �� �����
	//! \param Np ������� ��� �������� ��������
	//! \param NetAddr ����� �������
	void	SendCameraResponse(NetProto& Np, netadr_t NetAddr);

	//! ������� ��������� � ������ ������ ��������
	//! \param Np ������� ��� �������� ��������
	//! \param TimeFilm - ����� ������
	void	SendStartWork(NetProto& Np, const std::pair<CTime, CTime>& TimeFilm);

	//! ������� ��������� �� ��������� ������ ��������
	//! \param Np ������� ��� �������� ��������
	void	SendStopWork(NetProto& Np);

	//! ������� ����� �� �����
	void	TrySendDataResponse();

	//! ������� ������� ��������� �������
	void	SendCurrentState();

	//! ������� ����������� �� ������
	void	PumpImage();

	//! ������� ������� �����
	//! \param Np ������� ��� �������� ��������
	void	PumpMessage( NetProto& Np );

	//! ������ ���������� �� ��������
	void	UpdateStat();

	//! ������� ���������
	void	PumpState();

	//! ������ ����� ���-�� ��������
	void	StartProcessing();

	//! ������� ��������� ��� �������
	void	PopThreadMessage();

	//! ������� ��������� �� ������
	//! \param nCameraNo ������������� ������
	void	PushImageThreadMessage(int nCameraNo);

	//! ������� ��������� �� ����
	//! \param HallData ������ �� ����������� ����
	void	TrySendHallData( const std::vector<BYTE>& HallData );

	//! �������� �������� �������
	void	CheckChildThreads();

	void	WorkWithThreads();
	//! 
	//! \param Cameras 
	void	GetDataFromCameras( const std::set<int>& Cameras );

	//! ��������� ������ ��� ����������� ����
	void	LoadDataToHallAnalyzer();

	//! ���������� �� ������� ���������� ����?
	//! \return ��/���
	bool	IsLocalHallAnalyzer() const { return m_HallConfig.IsLocal(); }
	//! ���������� �� ������� ���������� ����?
	//! \return ��/���
	bool	IsMainServer() const 
	{ 
		return m_HallConfig.IsLocal() /*&& m_HallAnalyzer.get()*/; 
	}
	//! ��������� ������ ��� ����������� ����?
	//! \return ��/���
	bool	NeedNetworkRequest() const 
	{ 
		return m_State != ss_begin && m_HallConfig.IsLocal() && m_HallAnalyzer.get();
	}

	//! ���������� ������ ��������� ��� ������� ����
	//! \return DNS/IP address �������
	std::wstring	GetHallComputerName() const { return m_HallConfig.m_sHallUrl; }
	//! ����������� ������ �������
	//! \param cs ��������� �������
	//! \return ��������/�� ��������
	bool	CMainServerImpl::IsRemoteServerWorking( int cs) const
	{
		return	cs == INetProtoCallback::CurrentStateType::State_Working || 
			cs == INetProtoCallback::CurrentStateType::State_Processing;
	}

	//! �������� ������ ���������� �������
	//! \param cs ��������� �������
	//! \return ��������/�� ��������
	bool	CMainServerImpl::IsLocalServerWorking( int cs) const
	{
		return cs == ss_running;
	}

	//! �������� ��������� ���������
	void	RefreshAlgoParam();

	//! �������� ����������� ���
	//! \return ��� ����������
	std::wstring	CMainServerImpl::GetOwnName() const;


	//! ���-�� ��������, ����� ��� boost::optional
	class SpectatorDataOnServer
	{
		//! ���=�� ��������
		int		m_nNumber;
		//! ����, ������ �������������
		bool	m_bValid;
	public:

		//! ���������� ���-�� ��������
		void Set(int nSpectator)
		{
			m_nNumber	= nSpectator;
			m_bValid	= true;
		}
		//! ������ �������?
		//! \return ��/���
		bool	IsReady() const				{	return m_bValid; }
		//! ������ �� �������������
		void	Reset()						{	m_bValid = false; }
		//! ������� ���-�� ��������
		//! \return ���-�� ��������
		int		Get() const		{	return m_nNumber; }
	} m_Spectators;

	std::pair<CTime, CTime>		m_TimeFilm;

	void					SetFilmTime( const std::pair<CTime, CTime>& time ) { m_TimeFilm = time; }
	std::pair<CTime, CTime>	GetFilmTime( ) const { return m_TimeFilm; }
};

#endif //__MAIN_SERVER_IMPL_H__