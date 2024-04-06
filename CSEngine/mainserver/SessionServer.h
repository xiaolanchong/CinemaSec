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
#ifndef __SESSION_SERVER_H_INCLUDED_0663015230141648__
#define __SESSION_SERVER_H_INCLUDED_0663015230141648__

#include "../../CSChair/COMMON/Exception.h"

MACRO_EXCEPTION( SessionServerException, CommonException );
MACRO_EXCEPTION( SessionServerUnexpectedException,	SessionServerException );
MACRO_EXCEPTION( SessionServerNoEmptyException,		SessionServerException );
//=====================================================================================//
//                                 class SessionServer                                 //
//=====================================================================================//
class SessionServer
{
	GUID	m_SessionID;
	HANDLE	m_hRoomEvent;
public:
	SessionServer();
	virtual ~SessionServer();

	bool		IsNewSessionRequired( const GUID& SessionID); 
	void		StartServerSession();
	void		StartClientSession( const GUID& SessionID);
	const GUID&	GetSessionID() const { return m_SessionID; }

	// throws SessionServerException
	int			StartRoomSession( const std::vector<int>& RoomIDArr ); 
};

#endif //__SESSION_SERVER_H_INCLUDED_0663015230141648__