#include "StdAfx.h"
#include ".\netproto.h"

#include "../../CSChair/public/memleak.h"
/*
NetProto::NetProto(void)
{
}*/

NetProto::~NetProto(void)
{
}

void	NetProto::PumpMessage(	netadr_t NetAddr, const std::vector<BYTE>& Msg		)
{
	if( Msg.size() < sizeof(GenericHdr) )
	{
		// ignore
		m_pCallback->OnErrorMessage( NetAddr, &Msg[0], Msg.size() );
		return;
	}
	GenericHdr* pGenHdr = (GenericHdr*)&Msg[0];
	switch(pGenHdr->Command) 
	{
	case Hdr_RequestData:
		RequestData( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_ResponseData:
		ResponseData( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_StartWork:
		ReceiveStartWork( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_CurrentState:
		ReceiveCurrentState( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_StopWork:
		m_pCallback->OnStopWork( NetAddr);
	case Hdr_ImageAccStart:
		ReceiveImageAccStart( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_ImageAccStop:
		ReceiveImageAccStop( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_ImageAccResponse:
		ReceiveImageAccResponse( NetAddr,&Msg[0], Msg.size() );
		break;
	case Hdr_FileRequest:
		ReceiveFileSendRequest(  NetAddr,&Msg[0], Msg.size()  );
		break;
	case Hdr_FileResponse:
		ReceiveFileSendResponse(  NetAddr,&Msg[0], Msg.size()  );
		break;
	case Hdr_FileResponseEnd:
		ReceiveFileSendResponseEnd(  NetAddr,&Msg[0], Msg.size()  );
		break;
	case Hdr_HallData:
		ReceiveHallData(  NetAddr,&Msg[0], Msg.size()  );
		break;
	case Hdr_ExchangeReconnect:
	case Hdr_ExchangeReport:
	//	ReceiveOneCommand< PS_NetExchangeReconnect, INetProtoCallback::OnExchangeReconnect >( NetAddr,&Msg[0], Msg.size() );
		break;
	default: // unknown cmd
		;
	}
}

void	NetProto::RequestData( netadr_t NetAddr, const void* pBytes, size_t nSize )
{
	if( ( sizeof(RequestDataHdr) ) > nSize  ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	RequestDataHdr* pHdr = ( RequestDataHdr* )pBytes;

	std::vector<int> CamId;
	for( size_t i= 0; i < ( nSize - sizeof(GenericHdr) ) / sizeof(  pHdr->CameraID[0] ) ; ++i)
	{
		__int32 Id = pHdr->CameraID [i];
		CamId.push_back( Id );
	}

	m_pCallback->OnRequestData( NetAddr, CamId );
}

void	NetProto::ResponseData( netadr_t NetAddr, const void* pBytes, size_t nSize )
{
	std::vector< INetProtoCallback::ResponseDataStruct > CamData ;
	ResponseDataHdr* pHdr = ( ResponseDataHdr* )pBytes;
	
	ResponseDataHdr::CameraSubHdr* pSubHdr = pHdr->CameraHdr;
	std::vector<BYTE> ImgArr, DataArr;
	for( size_t i = 0; i < pHdr->CameraTotal; ++i )
	{
		DWORD nDataSize		= pSubHdr->DataSize;
		DWORD nImageSize	= pSubHdr->ImgSize;
		int CamId = pSubHdr->CameraID;
		DataArr.assign( pSubHdr->Data, pSubHdr->Data + nDataSize );
		ImgArr.assign(  pSubHdr->Data + nDataSize , pSubHdr->Data + nDataSize + nImageSize );
		CamData.push_back( INetProtoCallback::ResponseDataStruct(  CamId, DataArr, ImgArr )   );
		pSubHdr = (ResponseDataHdr::CameraSubHdr*)(
					(__int8*)pSubHdr + sizeof( ResponseDataHdr::CameraSubHdr ) +  nDataSize + nImageSize - sizeof(__int8)
					); 
	}
	m_pCallback->OnResponseData( NetAddr, CamData );
}

void	NetProto::ReceiveStartWork(  netadr_t NetAddr,const void* pBytes, size_t nSize )
{
	if( sizeof( StartHdr ) > nSize )
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	StartHdr* pHdr = ( StartHdr* )pBytes;
	m_pCallback->OnStartWork( NetAddr, pHdr->SessionID, pHdr->nStartSec, pHdr->nEndSec );
}

void	NetProto::ReceiveCurrentState(  netadr_t NetAddr,const void* pBytes, size_t nSize )
{
	if( sizeof( CurrentStateHdr ) > nSize ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}

	CurrentStateHdr* pHdr = ( CurrentStateHdr* )pBytes;

	GUID	SessionID	= pHdr->SessionID;
	int		nRoomID		= pHdr->nRoomID;
	__int64 nBeginSec	= pHdr->nBeginSec;
	__int64 nEndSec		= pHdr->nEndSec;
	std::vector< INetProtoCallback::CurrentStateType > StateArr;
	const CurrentStateHdr::SubHdr* pSubHdr = pHdr->Info;
	size_t nRemainSize = nSize - sizeof( CurrentStateHdr ) + sizeof ( CurrentStateHdr::SubHdr );
	size_t nStructs =  nRemainSize/sizeof ( CurrentStateHdr::SubHdr );
	for( size_t i = 0; i < nStructs; ++i )
	{
		StateArr.push_back( 
			INetProtoCallback::CurrentStateType( pSubHdr[i].SubType, pSubHdr[i].CameraID, pSubHdr[i].State ) );
	}
	m_pCallback->OnCurrentState( NetAddr, nRoomID, SessionID, nBeginSec, nEndSec, StateArr );
}

void	NetProto::ReceiveImageAccStart( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( ImageAccStartHdr ) > nSize ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	ImageAccStartHdr* pHdr = ( ImageAccStartHdr* )pBytes;
	INetProtoCallbackServer* pIntEx = dynamic_cast< INetProtoCallbackServer* >( m_pCallback );
	if( pIntEx ) pIntEx->OnImageAccStart( pHdr->nRoomID );
}

void	NetProto::ReceiveImageAccStop( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( ImageAccStopHdr ) > nSize ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	ImageAccStopHdr* pHdr = ( ImageAccStopHdr* )pBytes;
	INetProtoCallbackServer* pIntEx = dynamic_cast< INetProtoCallbackServer* >( m_pCallback );
	if( pIntEx ) pIntEx->OnImageAccStop( pHdr->bSaveImages > 0 );
}

void	NetProto::ReceiveImageAccResponse( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( ImageAccResponseHdr ) > nSize ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	ImageAccResponseHdr* pHdr = ( ImageAccResponseHdr* )pBytes;
	INetProtoCallbackServer* pIntEx = dynamic_cast< INetProtoCallbackServer* >( m_pCallback );
	if( pIntEx ) pIntEx->OnImageAccResponse( pHdr->nResult );
}

////////// file exchange ///////////

void	NetProto::ReceiveFileSendRequest( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( FileRequestHdr ) > nSize || !m_pFileCallback ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	else
	{
		FileRequestHdr* pHdr = ( FileRequestHdr* )pBytes;
		size_t nStartSize = sizeof (FileRequestHdr); 
		std::vector<int> CameraIDs; 
		const __int32* pStartCam = (const __int32*)((const BYTE*)pBytes + nStartSize);
		for (size_t i = 0; i < (nSize - sizeof (FileRequestHdr) )/sizeof(__int32); ++i) 
		{
			CameraIDs.push_back(pStartCam[i]);
		}
		
		m_pFileCallback->OnFileRequest( NetAddr, CameraIDs, pHdr->nBeginTime, pHdr->nEndTime );
	}
}

void	NetProto::ReceiveFileSendResponse (netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( FileResponseHdr ) > nSize || !m_pFileCallback) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	else
	{
		FileResponseHdr* pHdr = ( FileResponseHdr* )pBytes;
		m_pFileCallback->OnFileResponse( NetAddr, pHdr->nCameraID, pHdr->nFileSize, (BYTE*)pBytes + sizeof(FileResponseHdr), nSize - sizeof(FileResponseHdr) );
	}
}

void	NetProto::ReceiveFileSendResponseEnd( netadr_t NetAddr, const void* pBytes, size_t nSize  )
{
	if( sizeof( FileResponseEndHdr ) > nSize  || !m_pFileCallback) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	else
	{
		FileResponseEndHdr* pHdr = ( FileResponseEndHdr* )pBytes;
		m_pFileCallback->OnFileResponseEnd( NetAddr, pHdr->nCameraID, pHdr->nResult, pHdr->nCRC );
	}
}

void	NetProto::ReceiveHallData( netadr_t NetAddr,const void* pBytes, size_t nSize  )
{
	if( sizeof( HallDataHdr ) > nSize ) 
	{
		m_pCallback->OnErrorMessage( NetAddr, pBytes, nSize );
		return;
	}
	else
	{
		HallDataHdr* pHdr = ( HallDataHdr* )pBytes;
		std::vector<BYTE> buf( (const BYTE*)pBytes + sizeof(HallDataHdr), (const BYTE*)pBytes + nSize );
		m_pCallback->OnHallData( NetAddr, buf );
	}
}

