//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Every recognition session has own unique identifier, server send to multicast groups his uuid
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   02.03.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "SessionServer.h"
#include <strsafe.h>

//=====================================================================================//
//                                 class SessionServer                                 //
//=====================================================================================//
SessionServer::SessionServer() : m_hRoomEvent(0)
{
	HRESULT res = CoCreateGuid( &m_SessionID );
	ASSERT( res == S_OK );
}

SessionServer::~SessionServer()
{
	if( m_hRoomEvent ) CloseHandle(m_hRoomEvent);
}

bool	SessionServer::IsNewSessionRequired( const UUID& SessionID)
{
	bool res = IsEqualGUID( SessionID, m_SessionID ) != TRUE;
	m_SessionID = SessionID;
	return res;
}

void	SessionServer::StartServerSession()
{
	HRESULT res = CoCreateGuid( &m_SessionID );
	ASSERT( res == S_OK );
}

void	SessionServer::StartClientSession(const UUID& SessionID)
{
	m_SessionID = SessionID;
}

int		SessionServer::StartRoomSession( const std::vector<int> &RoomIDArr )
{
	TCHAR szName[ MAX_PATH ];

	for( size_t i = 0 ; i < RoomIDArr.size(); ++i )
	{
		HRESULT hr = StringCchPrintf( szName, MAX_PATH,
			_T("{C0664617-E896-4A05-B72C-EDEF4CC655E8}:Room%d"), RoomIDArr[i] ) ;
		if(hr != S_OK) throw SessionServerUnexpectedException("Format string error");
		HANDLE hEvent = OpenEvent( EVENT_ALL_ACCESS, FALSE, szName );
		if( !hEvent )
		{
			m_hRoomEvent = CreateEvent( NULL, TRUE, TRUE, szName );
			if( !m_hRoomEvent) throw SessionServerUnexpectedException("CreateEvent error");
			else return RoomIDArr[i];
		}
	}
	throw SessionServerNoEmptyException("No empty rooms");
}