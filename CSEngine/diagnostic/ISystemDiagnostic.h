//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Scan for network activity and report about current system state, 
//	for CSClient
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 10.02.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_SYSTEM_DIAGNOSTIC_7570009780418761_
#define _I_SYSTEM_DIAGNOSTIC_7570009780418761_
//======================================================================================//
//                               struct ISystemDiagnostic                               //
//======================================================================================//

#define SYSTEM_DIAGNOSTIC_INTERFACE	L"{68FF9A06-BB43-485F-8525-EC248C33F69C}"

struct DBTableRoomFacet;
struct DBTableCameraFacet;

struct ISystemDiagnostic
{
	enum DeviceState
	{
		ds_offline,
		ds_stopped,
		ds_working,
		ds_processing
	};

	virtual void	Release()										= 0;
	virtual HRESULT	Initialize( IDebugOutput* pDebugInt)			= 0;
	virtual HRESULT	GetComputerState( LPCWSTR szServerName, int nRoomID, DeviceState& st )	= 0;
	virtual HRESULT GetCameraState( int nCameraNo, DeviceState& st)				= 0;

	virtual HRESULT GetExchangeState( LPCWSTR szServerName, DeviceState& st )	= 0;
	virtual	HRESULT Report(LPCWSTR szServerName)								= 0;
	virtual	HRESULT Reconnect(LPCWSTR szServerName)								= 0;
};

inline void DestroyDiagnostic( ISystemDiagnostic* pInt )
{
	pInt->Release();
}

#endif // _I_SYSTEM_DIAGNOSTIC_7570009780418761_