//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Менеджер отладочного интерфейса IDebugOutput для клиента
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 24.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _CLIENT_LOG_MANAGER_1003276869306433_
#define _CLIENT_LOG_MANAGER_1003276869306433_

#include <queue>
#include <boost/tuple/tuple.hpp>
//======================================================================================//
//                                class ClientLogManager                                //
//======================================================================================//

struct IClientLogManagerCallback
{
	virtual void OnMessage( LPCWSTR szSystem, CTime timeMsg, int nSeverity, LPCWSTR szMsg ) = 0;
};

struct IClientLogManager
{
	typedef boost::tuple< std::wstring, CTime, int, std::wstring > Message_t;
	virtual IDebugOutput*						CreateDebugOutput( LPCWSTR szSystem ) = 0;
	virtual std::queue<Message_t>&				LockBuffer()	= 0;
	virtual void								UnlockBuffer()	= 0;
	virtual void								WriteMessage( const Message_t& msg) = 0;
};

IClientLogManager& GetLogManager();

#endif // _CLIENT_LOG_MANAGER_1003276869306433_