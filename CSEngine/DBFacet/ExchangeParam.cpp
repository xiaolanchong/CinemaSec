//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange database parameter stuff
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 31.05.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ExchangeParam.h"

struct DBA_ExchangeParam
{
	int				m_nTimeoutSec;
	int				m_nTimeoutNumber;
	int				m_nWaitTimeout;
    int				m_nStatTreshold;

	BEGIN_ACCESSOR_MAP(DBA_ExchangeParam, 1)
		BEGIN_ACCESSOR(0, true)
		COLUMN_ENTRY(1, m_nTimeoutSec )
		COLUMN_ENTRY(2, m_nTimeoutNumber )
		COLUMN_ENTRY(3, m_nWaitTimeout )
		COLUMN_ENTRY(4, m_nStatTreshold )
		END_ACCESSOR()
	END_ACCESSOR_MAP()
};

//======================================================================================//
//                                 class ExchangeParam                                  //
//======================================================================================//
ExchangeParam::ExchangeParam()
{
}

ExchangeParam::~ExchangeParam()
{
}

void	ExchangeParam::SetExchangeParam( int nRoomID, int nTimeoutSec, int nTimeoutNumber, int nWaitTimeout, int nStatTreshold )
{
	CCommand<CAccessor<DBA_ExchangeParam> >		A_ExchangeParam;
	FetchData( nRoomID, A_ExchangeParam, true );
	A_ExchangeParam.m_nTimeoutSec		= nTimeoutSec;
	A_ExchangeParam.m_nTimeoutNumber	= nTimeoutNumber;
	A_ExchangeParam.m_nWaitTimeout		= nWaitTimeout;
	A_ExchangeParam.m_nStatTreshold		= nStatTreshold;
	HRESULT hr = A_ExchangeParam.SetData();
	if( hr != S_OK )
	{
		TraceError(hr);
		throw TableException("Exchange view error");
	}
}

void	ExchangeParam::GetExchangeParam( int nRoomID, int& nTimeoutSec, int& nTimeoutNumber, int& nWaitTimeout, int& nStatTreshold )
{
	CCommand<CAccessor<DBA_ExchangeParam> >		A_ExchangeParam;
	FetchData( nRoomID, A_ExchangeParam, false );
	nTimeoutSec		= A_ExchangeParam.m_nTimeoutSec;
	nTimeoutNumber	= A_ExchangeParam.m_nTimeoutNumber;
	nWaitTimeout	= A_ExchangeParam.m_nWaitTimeout;
	nStatTreshold	= A_ExchangeParam.m_nStatTreshold;
}