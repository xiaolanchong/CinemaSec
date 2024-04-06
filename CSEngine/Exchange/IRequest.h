//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange between CSLauncher & CSChair data exchange protocol implementation interface
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 23.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_REQUEST_3145534737282962_
#define _I_REQUEST_3145534737282962_

#include "../../CSChair/debug_int.h"
//======================================================================================//
//                                    struct IRequest                                    //
//======================================================================================//

#define REQUEST_INTERFACE		L"{CAC315E8-25F6-4EFC-9F93-19367D0841B8}"

#define MAKE_REQUEST_ERROR( x )		MAKE_HRESULT( SEVERITY_ERROR, 201, x );

#define	E_NOT_CONNECTED				MAKE_REQUEST_ERROR(1)
#define	E_ALREADY_CONNECTED			MAKE_REQUEST_ERROR(2)

struct IRequest
{
	enum State
	{
		st_receive_msg,
		st_send_msg,
		st_wait_response,
		st_not_running,
		st_disconnect,
		st_try_connect,
	};
	
	virtual HRESULT	Start		( IDebugOutput* pInt ) 		= 0; 
	virtual HRESULT	Stop		()							= 0;
	virtual HRESULT	Request		( BSTR sXmlQuery ) 			= 0;
	virtual HRESULT Reconnect()								= 0;
	virtual HRESULT ForceReport()							= 0;
	virtual HRESULT GetState	( IRequest::State& st )		= 0;
	
	virtual void	Release		() 							= 0;
};

#endif // _I_REQUEST_3145534737282962_