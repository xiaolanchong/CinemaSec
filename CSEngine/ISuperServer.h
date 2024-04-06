//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	IMainServer's server for numerous rooms' processing on the computer
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.02.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_SUPER_SERVER_7234929717442610_
#define _I_SUPER_SERVER_7234929717442610_

#define		SUPER_SERVER_INTERFACE	L"{B2696CF4-E16D-41AC-B8EB-C152F82F3FB5}"

#include "../CSChair/debug_int.h"
//======================================================================================//
//                                 struct ISuperServer                                  //
//======================================================================================//

struct ICallbackServer
{
	enum
	{
		StateOk,
		StateDisconnected,
		StateNotCreated
	};
	virtual HRESULT __stdcall GetState(int& nState) = 0;
	virtual HRESULT __stdcall Reconnect() = 0;
	virtual HRESULT __stdcall Report() = 0;
};

struct ISuperServer
{
	virtual void	Release() = 0;

	virtual HRESULT	Initialize( IDebugOutput* pDbgInt, IDebugOutput* pGrabberDbgInt, bool bInitVideo )	= 0;	
	virtual HRESULT	SetCameraWindow( int nRoomID, int nCameraNo, HWND hWnd )			= 0; 

	virtual HRESULT StartServer		( int nRoomID, IDebugOutput *pInt, LPCWSTR szDirName ) = 0;
	virtual HRESULT StopServer		( int nRoomID )	= 0;					;
	virtual HRESULT RestartServer	( int nRoomID )	= 0;

	virtual void	RegisterCallback( ICallbackServer * pInt) = 0;
	virtual void	UnregisterCallback( ) = 0;
};

inline void	DestroySuperServer( ISuperServer* p )
{
	p->Release();
}

#endif // _I_SUPER_SERVER_7234929717442610_