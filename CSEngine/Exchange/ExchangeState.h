//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	States of the main working thread
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 25.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _EXCHANGE_STATE_2469062877500014_
#define _EXCHANGE_STATE_2469062877500014_

#include <boost/shared_ptr.hpp>
#include "../mainserver/ThreadError.h"
#include "IRequest.h"
//======================================================================================//
//                                 class ExchangeState                                  //
//======================================================================================//
class CRequestImpl;

class ExchangeState
{
protected:
	CRequestImpl*	m_pImpl;
public:
	ExchangeState( CRequestImpl*	pImpl ) : m_pImpl( pImpl) {} 
	static boost::shared_ptr<ExchangeState> GetStartState(CRequestImpl*);
	virtual ExchangeState*		Process() = 0;

	virtual LONG	GetStateName() const = 0;
	virtual ~ExchangeState() {};
};

class IncomingRequestState : public ExchangeState
{
	SimpleTimer				m_StatTimer;
	static const			c_StatPeriod = 5;
public:
	IncomingRequestState( CRequestImpl*	pImpl ) ;
	virtual ExchangeState*		Process() ;
	virtual LONG	GetStateName() const { return IRequest::st_receive_msg; };
	virtual ~IncomingRequestState() {};
};

class ConnectState : public ExchangeState
{
	int							m_nTicks;
	bool						m_bFirst;
	SimpleTimerWithPeriod		m_ConnectTimer;
public:
	ConnectState( CRequestImpl*	pImpl ) ;
	virtual ExchangeState*		Process() ;
	virtual LONG	GetStateName() const { return IRequest::st_try_connect; };
	virtual ~ConnectState() {};
};

class DisconnectState : public ExchangeState
{
public:
	DisconnectState( CRequestImpl*	pImpl ) ;
	virtual ExchangeState*		Process() ;
	virtual LONG	GetStateName() const { return IRequest::st_disconnect; };
	virtual ~DisconnectState() {};
};

class SendReportState : public ExchangeState
{
public:
	SendReportState( CRequestImpl*	pImpl ) ;
	virtual ExchangeState*		Process() ;
	virtual LONG	GetStateName() const { return IRequest::st_send_msg; };
	virtual ~SendReportState() {};
};

class WaitForResponseState : public ExchangeState
{
	SimpleTimer		m_WaitTimer;
public:
	WaitForResponseState( CRequestImpl*	pImpl ) ;
	virtual ExchangeState*		Process() ;
	virtual LONG	GetStateName() const { return IRequest::st_wait_response; };
	virtual ~WaitForResponseState() {};
};

#endif // _EXCHANGE_STATE_2469062877500014_