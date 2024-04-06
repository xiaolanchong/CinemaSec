#ifndef __MAIN_SERVER_H__
#define __MAIN_SERVER_H__

#include "syscfg_int.h"
#include "../../CSChair/debug_int.h"
#include "NetworkLayer.h"
#include "../../CSChair/Grabber/IGrabberFactory.h"

#define MAIN_SERVER_INTERFACE_0		L"{3FE31ADA-F4FE-414E-9FFA-272495D68BB8}"

struct	ICameraAnalyzer;
struct	IHallAnalyzer;

struct IMainServer
{
//	DebugMode
	virtual DWORD		SetDebugInterface(IDebugOutput* pDebugInt) = 0;
	virtual HRESULT		SetCameraWindow(int nCameraNo, HWND hWnd) = 0;
//	virtual HRESULT		SetHallWindow( HWND hWnd) = 0;

//	ControlMode
	virtual DWORD	Initialize( IGrabberFactory* pFactory, INetworkLayer* pNetInt )	= 0;
	virtual DWORD	Start	(bool bForce)		= 0;	// force 
	virtual DWORD	Stop	(bool bForce)		= 0;	// force
#if 0
	// for debug only
	virtual ICameraAnalyzer*	GetCameraAnalyzer(int ZoneNo)	= 0;
	virtual IHallAnalyzer*		GetHallAnalyzer()				= 0;
#endif
	virtual void	Release()	= 0;
};

#endif //__MAIN_SERVER_H__