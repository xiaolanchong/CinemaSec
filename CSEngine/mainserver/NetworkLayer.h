//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	intermediate layer for NetLink & NetTcpLink  simulation
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.02.2005
//                                                                                      //
//======================================================================================//
#ifndef _NETWORK_LAYER_3228154838026467_
#define _NETWORK_LAYER_3228154838026467_

#include "../IPC/NetTcpLink.h"
#include "../IPC/NetLink.h"

struct INetworkLayer
{
	// for server
	virtual void				LockIncomeBuffer(int nRoomID)	= 0;
	virtual void				UnlockIncomeBuffer(int nRoomID) = 0;
	virtual NetIncomeBuffer_t&	GetIncomeBuffer(int nRoomID)	= 0;

	virtual void				TCP_CloseClient( int nRoomID, netadr_t NetAddr ) = 0;
	virtual void				UDP_SendMessage(int nRoomID,  netadr_t NetAddr, const void* pBytes, size_t nSize  ) = 0;
	// for client 
	virtual void				TCP_SendFromClient( int nRoomID, netadr_t ServerAddress, const void* pData, size_t nSize ) = 0;
};

#endif // _NETWORK_LAYER_3228154838026467_