#ifndef __NETPROTO_H__
#define __NETPROTO_H__

#include "../ipc/NetAddr.h"

const int	c_FileTransferID = -99;

//! интефейс обработки сетевых пакетов
struct INetProtoCallback
{
	//! структура текущего состояния элемента
	struct CurrentStateType
	{
		//! вид элемента
		int		DevType;
		//! идентификатор, если есть
		int		Id;
		//! состояние
		int		State;
		CurrentStateType(int dt, int id, int st ):
		DevType( dt ), Id( id), State(st){}
		//! вид элемента
		enum
		{
			//! камеры
			SubTypeCamera	= 0,
			//! обработчик зала
			SubTypeHall		= 1,
			//! сервер
			SubTypeServer	= 2,
			//! обмен
			SubTypeExchange = 3
		};
		enum
		{
			//! не работает, не тсвязи, неисправен
			State_Dead			= 0,
			//! остановлен
			State_Stopped		= 1,
			//! работает
			State_Working		= 2,
			//! обработка
			State_Processing	= 3,
			//! неизвестно
			State_Unknown
		};
		//! состояние обмена данных
		enum
		{
			//! неиспавен
			StateExchange_Dead			= 0,
			//! отключен
			StateExchange_Disconnect	= 1,
			//! работает
			StateExchange_Working		= 2,
		};
	};

	//! ответ от камеры
	struct ResponseDataStruct
	{
		//! идентфикатор камеры
		int					nCameraNo;
		//! данные камеры
		std::vector<BYTE>	Data;
		//! изображение с камеры
		std::vector<BYTE>	Img;
		ResponseDataStruct(int No, const std::vector<BYTE>& d, const std::vector<BYTE>& i ):
		nCameraNo( No ), Data( d), Img(i){}
	};


	//! обработчик запроса данных камеры
	//! \param NetAddr адрес сервера
	//! \param CameraIDs список идентификаторов камер
	virtual void OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ) = 0;

	//! обработчик ответа от камер
	//! \param NetAddr адрес сервера, с которого пришла инф-я
	//! \param Data данные ответа
	virtual void OnResponseData( netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data ) = 0;

	//! обработчик сообщения о старте подсчета
	//! \param NetAddr адрес сервера, с которого пришла инф-я
	//! \param SessionID идентфикатор сессии
	virtual void OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec ) = 0;

	//!  обработчик сообщения о конце подсчета
	//! \param NetAddr адрес сервера, с которого пришла инф-я
	virtual void OnStopWork( netadr_t NetAddr )	= 0;

	//! обработчик сообщения состояния системы
	//! \param NetAddr адрес сервера, с которого пришла инф-я
	//! \param nRoomID идентфикатор зала
	//! \param SessionID идентфикатор сессии
	//! \param Devices список устройств
	virtual void OnCurrentState(netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
								__int64 nBeginSec, __int64 nEndSec,
								const std::vector< CurrentStateType >& Devices) = 0;
//	virtual void OnExchangeReconnect(netadr_t NetAddr) = 0;
//	virtual void OnExchangeReport(netadr_t NetAddr) = 0;
	// for UDP datagrams ( ping, start, state )

	//! послать UDP датаграмму
	//! \param NetAddr адрес сервера, которому предназначена датаграмма
	//! \param Msg сообщение
	//! \param nSize размер
	virtual void SendMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) = 0;
	// for TCP packets ( data, requests )
	//! послать TCP пакет
	//! \param NetAddr адрес сервера, которому предназначена датаграмма
	//! \param Msg сообщение
	//! \param nSize размер
	virtual void SendReliableMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) = 0;

	//! обработчик неизвестного сообщения
	virtual void OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  ) = 0;

	//! обпаботчик данных зала
	//! \param NetAddr адрес сервера, с которого пришла инф-я
	//! \param Data данные щзала
	virtual void OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  ) = 0;
};

//! обработчик накопления изображения
struct INetProtoCallbackServer : INetProtoCallback 
{
	virtual void OnRequestData( netadr_t NetAddr, const std::vector<int>& CameraIDs ){};
	virtual void OnResponseData( netadr_t NetAddr, const std::vector< ResponseDataStruct >& Data ){};
	virtual void OnStartWork( netadr_t NetAddr, const GUID& SessionID, __int64 nStartSec, __int64 nEndSec ) {};
	virtual void OnStopWork( netadr_t NetAddr )	{};
	virtual void OnCurrentState(netadr_t NetAddr, int nRoomID,  const GUID& SessionID, 
								__int64 nBeginSec, __int64 nEndSec,
								const std::vector< CurrentStateType >& Devices) {};
	// for UDP datagrams ( ping, start, state )
	virtual void SendMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) {};
	// for TCP packets ( data, requests )
	virtual void SendReliableMessage( netadr_t NetAddr, const void* Msg, size_t nSize ) {};
	virtual void OnErrorMessage(netadr_t NetAddr, const void* Msg, size_t nSize  ) {};

	virtual void OnHallData( netadr_t NetAddr, const std::vector<BYTE>& Data  ) {}
//	virtual void OnExchangeReconnect(netadr_t NetAddr) {};
//	virtual void OnExchangeReport(netadr_t NetAddr) {};

	//! обработчик сообщения старта накопления
	//! \param nRoomID идентификатор зала
	virtual void OnImageAccStart( int nRoomID ) = 0;

	//! обработчик сообщения старта накопления
	//! \param bSaveImage сохранить изображение в БД
	virtual void OnImageAccStop( bool bSaveImage ) = 0;

	//! результат накопления
	//! \param nResult результат
	virtual void OnImageAccResponse( int nResult ) = 0;
};

//! обработчик передачи файла
struct INetFileCallback
{

	//! запрос на файл
	//! \param NetAddr адрес сервера, с которого пришла инф-я 
	//! \param CameraIDs список камер, для которых передается архив
	//! \param nBeginTime время начала архива
	//! \param nEndTime время конца архива
	virtual void	OnFileRequest( netadr_t NetAddr, const std::vector<int>& CameraIDs, INT64 nBeginTime, INT64 nEndTime)		= 0;

	//! передача файла
	//! \param NetAddr адрес сервера, с которого пришла инф-я  
	//! \param nCameraID список камер, для которых передается архив 
	//! \param nFileSize общий размер файла
	//! \param pData данные
	//! \param nSize размер
	virtual void	OnFileResponse( netadr_t NetAddr, int nCameraID, DWORD64 nFileSize, const BYTE* pData, size_t nSize )		= 0;

	//! конец передачи файлов
	//! \param NetAddr адрес сервера, с которого пришла инф-я  
	//! \param nCameraID список камер, для которых передается архив  
	//! \param nResult результат передачи
	//! \param nCRC CRC файла
	virtual void	OnFileResponseEnd( netadr_t NetAddr, int nCameraID, UINT32 nResult, DWORD32 nCRC )		= 0;
};

//!
struct IPacketValidate
{
	//! данные сообщения
	std::vector<BYTE>	m_Message;
	//! валидация
	virtual void End() = 0;
};

class NetProto
{
	friend class NetRequest;
	friend class NetResponse;
public:
	enum 
	{
		Hdr_RequestData		= 10,
		Hdr_ResponseData	= 11,
		Hdr_HallData		= 12,
		Hdr_StartWork		= 50,
		Hdr_StopWork		= 51,
		Hdr_CurrentState	= 60,
		Hdr_ImageAccStart	= 70,
		Hdr_ImageAccStop	= 71,
		Hdr_ImageAccResponse= 72,
		Hdr_FileRequest			= 80,
		Hdr_FileResponse		= 81,
		Hdr_FileResponseEnd		= 82,
		Hdr_ExchangeReport		= 90,
		Hdr_ExchangeReconnect	= 91
	};

#pragma pack(1)
	struct GenericHdr
	{
		__int32		nReserved; // for downstream muxer
		__int32		Command;

		GenericHdr( ): nReserved( -1 ), Command(-1){}
	};

	struct RequestDataHdr : GenericHdr
	{
		__int32		CameraID[1];
	};

	struct CurrentStateHdr : GenericHdr
	{
		int			nRoomID;
		GUID		SessionID;
		__int64		nBeginSec;
		__int64		nEndSec;
		struct SubHdr
		{
			__int8				SubType;	// cam or hall
			__int32				CameraID;	// id if exists
			__int32				State;		// current state for this type
		};
		SubHdr	Info[1];
	};

	struct StopHdr : GenericHdr
	{
	};

	struct StartHdr : GenericHdr
	{
		GUID	SessionID;
		__int64	nStartSec;
		__int64	nEndSec;
	};

	struct ResponseDataHdr : GenericHdr
	{
		__int32					Result;
		unsigned __int32		CameraTotal;
		struct CameraSubHdr
		{
			__int32				CameraID;
			unsigned __int32	DataSize;
			unsigned __int32	ImgSize;
			__int8				Data[1];
		};

		CameraSubHdr	CameraHdr[1];
	};

	struct HallDataHdr : GenericHdr
	{

	};

	struct ImageAccStartHdr : GenericHdr
	{
		__int32	nRoomID;
	};

	struct ImageAccStopHdr : GenericHdr
	{
		__int8	bSaveImages;
	};

	struct ImageAccResponseHdr : GenericHdr
	{
		__int32	nResult;
	};

	struct FileRequestHdr : GenericHdr
	{
	//	unsigned __int32	nCameraTotal;
		INT64				nBeginTime;
		INT64				nEndTime;
	};

	struct FileResponseHdr : GenericHdr
	{
		__int32					nCameraID;
		unsigned __int64		nFileSize;
	};

	struct FileResponseEndHdr : GenericHdr
	{
		__int32					nCameraID;
		__int32					nResult;
		unsigned	__int32		nCRC;

		enum
		{
			Res_Ok				= 0,
			Res_Cancelled		= 1,
			Res_NoSuchCamera	= 2,
			Res_ServerBusy		= 3,
			Res_SourceError		= 4,
			Res_ClientCancel	= 5,
			Res_WrongTime		= 6,
			Res_WrongCamera		= 7
		};
	};

	struct P_ExchangeReconnect : GenericHdr
	{
	};

	struct P_ExchangeReport : GenericHdr
	{
	};

#pragma pack()
private:
	INetProtoCallback*	m_pCallback;
	INetFileCallback*	m_pFileCallback;

	void	RequestData(  netadr_t NetAddr,const void* pBytes, size_t nSize );
	void	ResponseData(  netadr_t NetAddr,const void* pBytes, size_t nSize );
	void	ReceiveImage(  netadr_t NetAddr,const void* pBytes, size_t nSize );
	void	ReceiveStartWork(  netadr_t NetAddr,const void* pBytes, size_t nSize );
	void	ReceiveCurrentState(  netadr_t NetAddr,const void* pBytes, size_t nSize );

	void	ReceiveHallData( netadr_t NetAddr,const void* pBytes, size_t nSize  );

	void	ReceiveImageAccStart( netadr_t NetAddr, const void* pBytes, size_t nSize  );
	void	ReceiveImageAccStop( netadr_t NetAddr, const void* pBytes, size_t nSize  );
	void	ReceiveImageAccResponse( netadr_t NetAddr, const void* pBytes, size_t nSize );

	void	ReceiveFileSendRequest( netadr_t NetAddr, const void* pBytes, size_t nSize  );
	void	ReceiveFileSendResponseEnd( netadr_t NetAddr, const void* pBytes, size_t nSize  );
	void	ReceiveFileSendResponse( netadr_t NetAddr, const void* pBytes, size_t nSize  );

	template <class T, void (INetProtoCallback:: *Fn)(netadr_t)> void ReceiveOneCommand(netadr_t NetAddr, const void* pBytes, size_t nSize)
	{
			if( sizeof( T ) > nSize ) 
			{
				m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
				return;
			}
			else
			{
				T* pHdr = ( T* )pBytes;
				(m_pCallback->*Fn)( NetAddr );
			}
	}

public:
	void	PumpMessage(	netadr_t NetAddr, const std::vector<BYTE>& Msg		);
	void	PushPacket( netadr_t NetAddr, IPacketValidate* p, bool bReliable)
	{
		p->End();
		if( bReliable )
			m_pCallback->SendReliableMessage( NetAddr, &p->m_Message[0], p->m_Message.size() );
		else
			m_pCallback->SendMessage( NetAddr, &p->m_Message[0], p->m_Message.size() );
	}
	

	NetProto(INetProtoCallback * cb, INetFileCallback* pfcb = NULL) : m_pCallback(cb), m_pFileCallback(pfcb){};
	~NetProto(void);
};

template<class T, int Cmd> class NetOneCommandRequest: public IPacketValidate
{
public:
	NetOneCommandRequest()
	{
		m_Message.resize(sizeof(T) );
		T* pHdr = (T*)&m_Message[0];
		pHdr->Command = Cmd;
	}
	void	End()	{	};
};

class NetRequest : public IPacketValidate
{
public:
	NetRequest( const std::vector<int> & Cameras )
	{
		m_Message.resize( sizeof(NetProto::GenericHdr) + 
							Cameras.size() * sizeof(__int32));	
		NetProto::RequestDataHdr* pHdr = (NetProto::RequestDataHdr*)&m_Message[0];
		for( size_t i = 0; i < Cameras.size(); ++i )
		{
			pHdr->CameraID[i] = Cameras[i];
		}
		pHdr->Command = NetProto::Hdr_RequestData;
	}
	virtual void	End()
	{
	}
};

class NetResponse: public IPacketValidate
{
	size_t				m_Number;
public:
	NetResponse() :  m_Number(0)
	{
		m_Message.resize( sizeof(NetProto::ResponseDataHdr) - sizeof( NetProto::ResponseDataHdr::CameraSubHdr ) );
	}
	void	AddCamera( int nCameraID,  int nResult, const std::vector<BYTE>& Data, const std::vector<BYTE>& Img  )
	{
#if 1
		size_t	nSizeDbg = m_Message.size();
#endif
		NetProto::ResponseDataHdr* pHdr = (NetProto::ResponseDataHdr*)&m_Message[0];
		NetProto::ResponseDataHdr::CameraSubHdr csh;
		csh.CameraID	= nCameraID;
		csh.DataSize	= (unsigned __int32)Data.size();
		csh.ImgSize		= (unsigned __int32)Img.size();
		m_Message.insert( m_Message.end(), (const BYTE*)&csh, (const BYTE*)&csh + sizeof(csh) - 1 );
		m_Message.insert( m_Message.end(), Data.begin() , Data.end() );
		m_Message.insert( m_Message.end(), Img.begin() , Img.end() );
#if 1
		nSizeDbg = m_Message.size();
#endif
		++m_Number;
	}
	void	End()
	{
		NetProto::ResponseDataHdr* pHdr	= (NetProto::ResponseDataHdr*)&m_Message[0];
		pHdr->Command			= NetProto::Hdr_ResponseData;
		pHdr->CameraTotal		= (unsigned __int32)m_Number;
#if 1
		BYTE*	pMsg = &m_Message[0];
		size_t	nSize = m_Message.size();
#endif
		
	}
};

class NetStart : public IPacketValidate
{
public:
	NetStart( const GUID& SessionID, __int64 nBeginSec, __int64 nEndSec) 
	{
		m_Message.resize( sizeof( NetProto::StartHdr ) );
		NetProto::StartHdr* pHdr = reinterpret_cast<NetProto::StartHdr*>( &m_Message[0] );
		pHdr->Command	= NetProto::Hdr_StartWork;
		pHdr->SessionID	= SessionID;
		pHdr->nStartSec	= nBeginSec;
		pHdr->nEndSec	= nEndSec;
	}
	void End(){}
};

class NetCurrentState : public IPacketValidate
{
public:
	NetCurrentState(  int nRoomID, int nServerState, const GUID & SessionID,
						__int64 nBeginSec, __int64 nEndSec)
	{
		m_Message.resize( sizeof( NetProto::CurrentStateHdr ) );
		NetProto::CurrentStateHdr* pHdr = (NetProto::CurrentStateHdr*)&m_Message[0];
		pHdr->Command	= NetProto::Hdr_CurrentState;
		pHdr->SessionID	= SessionID;
		pHdr->nRoomID	= nRoomID;
		pHdr->nBeginSec	= nBeginSec;
		pHdr->nEndSec	= nEndSec;
		pHdr->Info[0].SubType	= INetProtoCallback::CurrentStateType::SubTypeServer;
		pHdr->Info[0].State		= nServerState;
		pHdr->Info[0].CameraID	= -1;
	}
	void AddCamera( int nCameraNo, int nState  )
	{
		m_Message.resize(m_Message.size() + sizeof( NetProto::CurrentStateHdr::SubHdr ) );
		NetProto::CurrentStateHdr::SubHdr* pHdr = 
			(NetProto::CurrentStateHdr::SubHdr*)&m_Message[m_Message.size() - sizeof(NetProto::CurrentStateHdr::SubHdr)];
		pHdr->CameraID	= nCameraNo;
		pHdr->State		= nState;
		pHdr->SubType	= INetProtoCallback::CurrentStateType::SubTypeCamera;
	}
	void AddHall( int nState )
	{
		m_Message.resize(m_Message.size() + sizeof( NetProto::CurrentStateHdr::SubHdr ) );
		NetProto::CurrentStateHdr::SubHdr* pHdr = 
			(NetProto::CurrentStateHdr::SubHdr*)&m_Message[m_Message.size() - sizeof(NetProto::CurrentStateHdr::SubHdr)];
		pHdr->CameraID	= -1;
		pHdr->State		= nState;
		pHdr->SubType	= INetProtoCallback::CurrentStateType::SubTypeHall;
	}
	void End()
	{

	}
};

class NetHallData : public IPacketValidate
{
public:
	NetHallData( const std::vector<BYTE>& Data )
	{
		m_Message.resize( sizeof( NetProto::HallDataHdr ) );
		NetProto::HallDataHdr* pHdr = reinterpret_cast<NetProto::HallDataHdr*>( &m_Message[0] );
		pHdr->Command	= NetProto::Hdr_HallData;
		m_Message.insert( m_Message.end(), Data.begin(), Data.end() );
	}
	void End(){}
};

///////// file exchange ///////////

class NetFileRequest : public IPacketValidate
{
public:
	NetFileRequest( INT64 nBeginTime, INT64 nEndTime, const std::vector<int>& CameraIDs ) 
	{
		m_Message.resize( sizeof( NetProto::FileRequestHdr ) );
		NetProto::FileRequestHdr* pHdr = reinterpret_cast<NetProto::FileRequestHdr*>( &m_Message[0] );
		pHdr->Command	= NetProto::Hdr_FileRequest;
		pHdr->nBeginTime	= nBeginTime;
		pHdr->nEndTime		= nEndTime;

		for ( size_t i =0; i < CameraIDs.size(); ++i)
		{
			__int32 nId= CameraIDs[i];
			m_Message.insert( m_Message.end(), (const BYTE*)&nId, (const BYTE*)&nId + sizeof(__int32) );
		}
	}
	void End(){}
};

class NetSendFilePortion : public IPacketValidate
{
public:
	NetSendFilePortion( int nCameraID, DWORD64 nTotalSize, const void* pData, size_t nSize ) 
	{
		m_Message.resize( sizeof( NetProto::FileResponseHdr ) );
		NetProto::FileResponseHdr* pHdr = reinterpret_cast<NetProto::FileResponseHdr*>( &m_Message[0] );
		pHdr->Command	= NetProto::Hdr_FileResponse;
		pHdr->nCameraID	= nCameraID;
		pHdr->nFileSize = nTotalSize;
		m_Message.insert( m_Message.end(), (const BYTE*)pData, (const BYTE*)pData + nSize );
	}
	void End(){}
};

class NetSendFileEnd: public IPacketValidate
{
public:
	NetSendFileEnd( int nCameraID, UINT32 nResult, DWORD32 nCRC ) 
	{
		m_Message.resize( sizeof( NetProto::FileResponseEndHdr ) );
		NetProto::FileResponseEndHdr* pHdr = reinterpret_cast<NetProto::FileResponseEndHdr*>( &m_Message[0] );
		pHdr->Command	= NetProto::Hdr_FileResponseEnd;
		pHdr->nCameraID	= nCameraID;
		pHdr->nResult	= nResult;
		pHdr->nCRC		= nCRC;
		//FIXME
		pHdr->nReserved	= c_FileTransferID;
	}
	void End(){}
};

template< const int Res > class NetSendFileEndError : public NetSendFileEnd
{
public:
	NetSendFileEndError() :
		NetSendFileEnd( 0, Res, 0 ) 
		{}
};

typedef NetSendFileEndError<NetProto::FileResponseEndHdr::Res_ClientCancel>		NetSendFileClientCancel;
typedef NetSendFileEndError<NetProto::FileResponseEndHdr::Res_ServerBusy>		NetSendFileServerBusy;
typedef NetSendFileEndError<NetProto::FileResponseEndHdr::Res_WrongTime>		NetSendFileWrongTime;
typedef NetSendFileEndError<NetProto::FileResponseEndHdr::Res_WrongCamera>		NetSendFileWrongCamera;

typedef NetOneCommandRequest< NetProto::StopHdr,  NetProto::Hdr_StopWork >					NetStop;
typedef NetOneCommandRequest< NetProto::StopHdr,  NetProto::Hdr_ExchangeReport >			PS_NetExchangeReport;
typedef NetOneCommandRequest< NetProto::StopHdr,  NetProto::Hdr_ExchangeReconnect >			PS_NetExchangeReconnect;

#endif //__NETPROTO_H__