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
#ifndef _EXCHANGE_PARAM_4847339513319568_
#define _EXCHANGE_PARAM_4847339513319568_

#include "DBRows.h"
#include "DBHelper.h"
#include "OleDBConnection.h"

//======================================================================================//
//                                 class ExchangeParam                                  //
//======================================================================================//

class ExchangeParam:	protected virtual COleDBConnection,
						protected DBExchangeFacet
{
	template<typename T> void			FetchData( int nRoomID, T& A_ExchangeParam, bool bForUpdate )
	{
		HRESULT hr;
//		typename CCommand<CAccessor<T> > A_ExchangeParam;
		if( !m_dbSession.m_spOpenRowset )
		{
			throw TableException("Time table connection error");
		}
		CDBPropSet  propset(DBPROPSET_ROWSET);
		if( bForUpdate )
		{
			propset.AddProperty(DBPROP_IRowsetChange, true);
			propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );
		}
		CString sQuery;
		sQuery.Format(	_T("SELECT ExchangeTimeoutPeriod, ExchangeTimeoutNumber, ExchangeWaitTimeout, ExchangeStatTreshold ")
						_T("FROM ExchangeView WHERE RoomID=%d "), 
						nRoomID );

		hr = A_ExchangeParam.Open(m_dbSession, sQuery, bForUpdate? &propset : NULL);
		if( hr != S_OK )
		{
			TraceError(hr);
			throw TableException("Statistics table connection error");
		}

		if( A_ExchangeParam.MoveNext() != S_OK )
		{
			throw TableException("No such data");
		}
	}
public:
	ExchangeParam();
	virtual ~ExchangeParam();

	virtual void	SetExchangeParam( int nRoomID, int nTimeoutSec, int nTimeoutNumber, int nWaitTimeout, int nStatTreshold ) ;
	virtual void	GetExchangeParam( int nRoomID, int& nTimeoutSec, int& nTimeoutNumber, int& nWaitTimeout, int& nStatTreshold ) ;
};

#endif // _EXCHANGE_PARAM_4847339513319568_