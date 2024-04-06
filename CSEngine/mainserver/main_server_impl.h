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
//!		\li 2005-10-26	кол-во зрителей передается через очередь сообщений
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

	//! состояние сервера
	enum ServerState
	{
		//! старт
		ss_begin,
		//! инициализирован
		ss_initialized,
		//! запущен
		ss_running,
		//! остановлен
		ss_stopped
	};

	//! состояние, ServerState 
	LONG	m_State;

	//! обработчика запроса данных от камер (сетевой пакет)
	virtual void OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ) ;
	//! обработчика ответа от камер (сетевой пакет)
	virtual void OnResponseData(netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data );
	//! обработчик начала сеанса (сетевой пакет)
	virtual void OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec );
	//! обработчик окончания сеанса (сетевой пакет)
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

	//! установить отладочный интерфейс
	//! \param pDebugInt отладочный интерфейс
	//! \return 0 - успех, иначе - ошибка
	virtual DWORD		SetDebugInterface(IDebugOutput* pDebugInt) ;
	//! установить окно для отрисовки изображения камеры (отладка)
	//! \param nCameraNo идентификатор камеры
	//! \param hWnd окно
	//! \return S_OK - успех, иначе - ошибка
	virtual HRESULT		SetCameraWindow(int nCameraNo, HWND hWnd) ;

	//	ControlMode
	//! инициализация работы
	//! \param pFactory фабрика видеопотоков
	//! \param pNetInt сетевой интерфейс
	//! \return 0 - успех, иначе - ошибка
	virtual DWORD	Initialize(IGrabberFactory* pFactory, INetworkLayer* pNetInt )	;
	//! стартовать сервер
	//! \param bForce принудительно
	//! \return 0 - успех, иначе - ошибка
	virtual DWORD	Start	(bool bForce)		;	
	//! остановить сервер
	//! \param bForce принудительно
	//! \return 0 - успех, иначе - ошибка
	virtual DWORD	Stop	(bool bForce)		;	

	//! получить интерфейс камеры, для отладки
	//! \param ZoneNo идентификатор камеры
	//! \return интерфейс
	virtual ICameraAnalyzer*	GetCameraAnalyzer(int ZoneNo)	;
	//! получить интерфейс обработчика зала, для отладки
	//! \return интефейс
	virtual IHallAnalyzer*		GetHallAnalyzer()				;

	virtual void	Release()	;

	//! инициализация
	//! \return 0 - успех, иначе - ошибка
	virtual DWORD	InitializeWork()	;

	//! получить конфигурацию сервера
	//! \return 0 - успех, иначе - ошибка
	DWORD	GetDBConfig();

	//! стартовать подсчет
	//! \param bForce принудительно или по расписанию
	//! \return 0 - успех, иначе - ошибка
	DWORD	StartWork(bool bForce);
	DWORD	StopWork(bool bForce);
private:
	//! идентификатор обрабатываемого зала
	int									m_nRoomID;
	//! список удаленных камер
	std::vector<RemoteCameraConfig>		m_RemoteCameras;
	//! конфигурация обработчика
	HallConfig							m_HallConfig;
	//! сетевая конфигурация, порты, хост
	NetConfig							m_NetConfig;

	//! синхронизация потока
	ThreadSync							m_MainThread;
	//! синхронизация доступ к интерфейсам 
	SyncCriticalSection					m_AnalyzerCS;

	//! for network
	CameraDataBuffer					m_ServerCameraData;
	//! for threads
	CameraDataBuffer					m_ClientCameraData;

	//! список всех камер зала
	std::vector<int>					GetAllCameras() const;
	//! поставщик расписание, внесение статистки
	boost::shared_ptr<IStatisticsServer>	m_StatServer;
	//! состояние, в зависисмости от расписания
	std::auto_ptr<TimeAction>				m_pTimeAction;
	//! хранит текущее расписание
	TimeSheduler							m_TimeSheduler;
	//! использует расписание или стартовал принудительно, \sa StartWork
	bool									m_bUseTimeTable;
	//! сетевой интерфейс
	INetworkLayer*						m_pNetworkInt;
	//! обертка для формата сетевого протокола
	NetProto							m_NetProtocol;

	//! ф-я потока
	//! \param pParam параметр для потока
	//! \return 0 - успех, иначе ошибка
	static unsigned int WINAPI ServerThreadProc(void * pParam);
	//! защищенная try-catch ф-я потока, обертка ServerThreadProc
	//! \param pParam параметр для потока
	//! \return 0 - успех, иначе ошибка
	static unsigned int WINAPI Handled_ServerThreadProc(void * pParam);

	//! стартовать сервер
	//! \return 0 - успех, иначе ошибка
	DWORD	StopServer();
	//! остановить сервер
	//! \return 0 - успех, иначе ошибка
	DWORD	StartServer();

	//! отправить запрос на информацию о камерах
	//! \param Np обертка над сетевыми пакетами
	//! \param Host2Cameras словарь имя сервера-список его камер
	void	SendCameraRequest(NetProto& Np, const std::map < std::wstring, std::vector<int> >& Host2Cameras);

	//! послать ответ от камер
	//! \param Np обертка над сетевыми пакетами
	//! \param NetAddr адрес сервера
	void	SendCameraResponse(NetProto& Np, netadr_t NetAddr);

	//! послать сообщенеи о начале сеанса подсчета
	//! \param Np обертка над сетевыми пакетами
	//! \param TimeFilm - время сеанса
	void	SendStartWork(NetProto& Np, const std::pair<CTime, CTime>& TimeFilm);

	//! послать сообщенеи об окончании сеанса подсчета
	//! \param Np обертка над сетевыми пакетами
	void	SendStopWork(NetProto& Np);

	//! послать ответ от камер
	void	TrySendDataResponse();

	//! послать текущее состояние сервера
	void	SendCurrentState();

	//! извлечь изображения из пакета
	void	PumpImage();

	//! извлечь сетевой пакте
	//! \param Np обертка над сетевыми пакетами
	void	PumpMessage( NetProto& Np );

	//! внести статистику по подсчету
	void	UpdateStat();

	//! извлечь состояние
	void	PumpState();

	//! начало сбора кол-ва зрителей
	void	StartProcessing();

	//! извлечь сообщение для сервера
	void	PopThreadMessage();

	//! послать сообщение от камеры
	//! \param nCameraNo идентификатор камеры
	void	PushImageThreadMessage(int nCameraNo);

	//! послать сообщение от зала
	//! \param HallData данные от обработчика зала
	void	TrySendHallData( const std::vector<BYTE>& HallData );

	//! проверка дочерних потоков
	void	CheckChildThreads();

	void	WorkWithThreads();
	//! 
	//! \param Cameras 
	void	GetDataFromCameras( const std::set<int>& Cameras );

	//! загрузить данные для обработчика зала
	void	LoadDataToHallAnalyzer();

	//! существует на сервере обработчик зала?
	//! \return да/нет
	bool	IsLocalHallAnalyzer() const { return m_HallConfig.IsLocal(); }
	//! существует на сервере обработчик зала?
	//! \return да/нет
	bool	IsMainServer() const 
	{ 
		return m_HallConfig.IsLocal() /*&& m_HallAnalyzer.get()*/; 
	}
	//! необходим запрос для обработчика зала?
	//! \return да/нет
	bool	NeedNetworkRequest() const 
	{ 
		return m_State != ss_begin && m_HallConfig.IsLocal() && m_HallAnalyzer.get();
	}

	//! определить сервер обработки для данного зала
	//! \return DNS/IP address сервера
	std::wstring	GetHallComputerName() const { return m_HallConfig.m_sHallUrl; }
	//! определение работы сервера
	//! \param cs состояние сервера
	//! \return работает/не работает
	bool	CMainServerImpl::IsRemoteServerWorking( int cs) const
	{
		return	cs == INetProtoCallback::CurrentStateType::State_Working || 
			cs == INetProtoCallback::CurrentStateType::State_Processing;
	}

	//! проверка работы локального сервера
	//! \param cs состояние сервера
	//! \return работает/не работает
	bool	CMainServerImpl::IsLocalServerWorking( int cs) const
	{
		return cs == ss_running;
	}

	//! обновить параметры алгоритма
	void	RefreshAlgoParam();

	//! получить собственное имя
	//! \return имя компьютера
	std::wstring	CMainServerImpl::GetOwnName() const;


	//! кол-во зрителей, можно как boost::optional
	class SpectatorDataOnServer
	{
		//! кол=во зрителей
		int		m_nNumber;
		//! флаг, данные действительны
		bool	m_bValid;
	public:

		//! установить кол-во зрителей
		void Set(int nSpectator)
		{
			m_nNumber	= nSpectator;
			m_bValid	= true;
		}
		//! данные валидны?
		//! \return да/нет
		bool	IsReady() const				{	return m_bValid; }
		//! данные не действительны
		void	Reset()						{	m_bValid = false; }
		//! вернуть кол-во зрителей
		//! \return кол-во зрителей
		int		Get() const		{	return m_nNumber; }
	} m_Spectators;

	std::pair<CTime, CTime>		m_TimeFilm;

	void					SetFilmTime( const std::pair<CTime, CTime>& time ) { m_TimeFilm = time; }
	std::pair<CTime, CTime>	GetFilmTime( ) const { return m_TimeFilm; }
};

#endif //__MAIN_SERVER_IMPL_H__