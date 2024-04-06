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
#include "stdafx.h"
#include "ExchangeState.h"
#include "RequestImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//======================================================================================//
//                                 class ExchangeState                                  //
//======================================================================================//

IncomingRequestState::IncomingRequestState( CRequestImpl*	pImpl ) : 
	ExchangeState( pImpl)  ,
	m_StatTimer( c_StatPeriod * 1000 )
{
	pImpl->m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<NORMAL>" );
	m_StatTimer.Start();
} 

ExchangeState*		IncomingRequestState::Process() 
{
	m_pImpl->PopMessage();
	bool res = m_pImpl->SendOutputQueue();
	if( !res ) 
	{
		m_pImpl->DisconnectCOM();
		return new ConnectState ( m_pImpl ); 
	}
	else if( m_StatTimer.IsSignaled() )
	{
		m_StatTimer.Start();
		return  m_pImpl->IsTimeForStatReport(c_StatPeriod) ? new SendReportState( m_pImpl ) : NULL;
	}
	else
		return NULL;
}

/////////////////////////////////////////////////

ConnectState::ConnectState( CRequestImpl*	pImpl ) : 
	ExchangeState( pImpl),
	m_nTicks(0),
	m_ConnectTimer( m_pImpl->m_nExchangeTimeout * 1000 )
{
	pImpl->m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<CONNECT>" );
	
} 

ExchangeState*		ConnectState::Process() 
{
	if( m_nTicks == 0 )
	{
		bool res = m_pImpl->ConnectCOM();
		m_bFirst = false;
		++m_nTicks;
		if(res) return new IncomingRequestState( m_pImpl ) ;
		else 
		{
			m_ConnectTimer.Start( m_pImpl->m_nExchangeTimeout * 1000 );
			return NULL;	
		}
	}
	else if( !m_pImpl->m_nExchangeNumber )
	{
		m_nTicks = 0;
	}
	else if( m_nTicks >= m_pImpl->m_nExchangeNumber )
	{
		return new DisconnectState(m_pImpl);
	}
	if( m_ConnectTimer.IsSignaled() )
	{
		m_ConnectTimer.Start( m_pImpl->m_nExchangeTimeout * 1000 );
		++m_nTicks;
		bool res = m_pImpl->ConnectCOM();
		return res ? new IncomingRequestState( m_pImpl ) : NULL;	
	}
	return NULL;
}

//////////////////////////////////////

boost::shared_ptr<ExchangeState> ExchangeState::GetStartState( CRequestImpl* p)
{
	return boost::shared_ptr<ExchangeState>( new ConnectState(p) );
}

//////////////////////////////////////////////////////////////////////////

DisconnectState::DisconnectState( CRequestImpl*	pImpl ) : ExchangeState( pImpl) 
{
	pImpl->m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<DISCONNECT>" );
} 

ExchangeState*		DisconnectState::Process()
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

SendReportState::SendReportState( CRequestImpl*	pImpl ) : ExchangeState( pImpl) 
{
	pImpl->m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<SEND STATISTICS>" );
} 

ExchangeState*		SendReportState::Process()
{
	if( !m_pImpl->CreateStatReport() ) return NULL;
	if( m_pImpl->SendOutputQueue() ) 
		return new WaitForResponseState( m_pImpl ) ;
	else
	{
		m_pImpl->DisconnectCOM();
		return new ConnectState ( m_pImpl ); 
	}
}

WaitForResponseState::WaitForResponseState( CRequestImpl*	pImpl ) : ExchangeState( pImpl) ,
	m_WaitTimer( pImpl->m_nWaitTimeout * 1000 )
{
	m_WaitTimer.Start();
	pImpl->m_Debug.PrintW( IDebugOutput::mt_debug_info, L"<WAIT FOR RESPONSE> %d sec", pImpl->m_nWaitTimeout );
} 

ExchangeState*		WaitForResponseState::Process()
{
	bool res = m_pImpl->PopResponseMessage();
	if( res ) return new IncomingRequestState(m_pImpl);
	if( m_WaitTimer.IsSignaled() )
	{
		m_pImpl->m_Debug.PrintW( IDebugOutput::mt_error, L"Timeout while waiting a response" );
		return new IncomingRequestState(m_pImpl);
	}
	else
	{
		return NULL;
	}
}