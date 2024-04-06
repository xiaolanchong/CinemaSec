//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange component singleton
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 31.05.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "Exchange.h"
#include <boost/shared_ptr.hpp>
#include "../../CSEngine/Exchange/IRequest.h"
#include "../../CSEngine/CSEngine.h"
#include "../../CSEngine/ISuperServer.h"
//======================================================================================//
//                                    class Exchange                                    //
//======================================================================================//

static boost::shared_ptr< IRequest > s_pRequest;

// not reenterable

void	CreateExchange()
{
#if 1
	IRequest* pRequest = NULL;
	CreateEngineInterface( REQUEST_INTERFACE, (void**)&pRequest );
	s_pRequest = boost::shared_ptr<IRequest>( pRequest, ReleaseInterface<IRequest>() );
#endif
}

void	StartExchange( IDebugOutput *pDbgInt)
{
	if( s_pRequest.get() )
	{
		s_pRequest->Start( pDbgInt );
	}
}

void	DestroyExchange()
{
	if( s_pRequest.get() )
	{
		s_pRequest->Stop( );
	}
	s_pRequest.reset();
}

HRESULT	RequestExchange( BSTR sQuery )
{
	if( s_pRequest.get() )
	{
		return s_pRequest->Request( sQuery );
	}
	else
		return E_FAIL;
}

/////////////////////////////

class CallbackServerImpl : public ICallbackServer
{
	virtual HRESULT __stdcall GetState(int& nState)
	{
		if( !s_pRequest.get() ) return E_FAIL;
		IRequest::State st;
		HRESULT hr = s_pRequest->GetState( st );
		if( hr != S_OK ) return hr;
		switch(st)
		{
		case IRequest::st_receive_msg:
		case IRequest::st_send_msg:
		case IRequest::st_wait_response:
			nState = StateOk;
			break;
		case IRequest::st_not_running:
			nState = StateNotCreated;
			break;
		case IRequest::st_disconnect:
		case IRequest::st_try_connect:
		default:
			nState = StateDisconnected;
			break;
		}
		return S_OK;
	}

	virtual HRESULT __stdcall Reconnect()
	{
		if( s_pRequest.get() )
		{
			return s_pRequest->Reconnect(  );
		}
		else return E_FAIL;
	}

	virtual HRESULT __stdcall Report()
	{
		if( s_pRequest.get() )
		{
			return s_pRequest->ForceReport(  );
		}
		else return E_FAIL;
	}
};

///////////////////////////

ICallbackServer*	GetCallbackServerInt()
{
	static CallbackServerImpl s;
	return &s;
}