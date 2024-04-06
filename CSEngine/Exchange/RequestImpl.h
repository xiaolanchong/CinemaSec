//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange between CSLauncher & CSChair data exchange protocol implementation
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 23.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _REQUEST_IMPL_7171562827710921_
#define _REQUEST_IMPL_7171562827710921_

#include "IRequest.h"
#include "../mainserver/ThreadError.h"
#include "ExchangeState.h"
#include "../DBFacet/CinemaOleDb.h"
#include <boost/shared_ptr.hpp>

#if 1
#include "BoxOffice_h.h"
#else
#include "..\..\CSExchangeTest\CSExchangeTest_i.h"
#endif

#include "inputmessage.h"
#include "QueryException.h"

//======================================================================================//
//                                  class RequestImpl                                   //
//======================================================================================//

typedef std::pair<CTime, std::wstring>	QueueItem_t;
typedef std::list<QueueItem_t>			MessageQueue_t;

class CRequestImpl : public IRequest
{
	friend class IncomingRequestState;
	friend class SendReportState;
	friend class ConnectState;
	friend class DisconnectState;
	friend class WaitForResponseState;
public:
	CRequestImpl();
	virtual ~CRequestImpl();

private:
	virtual HRESULT	Start	( IDebugOutput* pInt ) ;
	virtual HRESULT	Stop();
	virtual HRESULT	Request		( BSTR sXmlQuery ) 		;
	virtual HRESULT Reconnect	();
	virtual HRESULT ForceReport()		;
	virtual HRESULT GetState	( IRequest::State& st )		;

	virtual void	Release() 	{ delete this; }

	unsigned int ThreadProc();
	static unsigned int WINAPI Handled_ThreadProc(void * pParam);

	void		PopMessage();
	bool		PopResponseMessage();
	std::pair<bool, bool>		ProcessQuery( CTime timeQuery, const std::wstring& sMsg );
	boost::shared_ptr<OutputMessage>	CreateErrorMessage( int nErrorCode, UINT nResourceID, QueryException& ex ); 
	void		RenewParam();
	void		SetDefaultParameters();
	void		ReconnectDB();

	bool		SendOutputQueue();
	bool		ConnectCOM();
	void		DisconnectCOM();

	bool		IsTimeForStatReport(int nPeriodSec);
	bool		CreateStatReport();

	void		LogMessageToDB( CCinemaOleDB& db, DBProtocolTable::MsgType mt, CTime time, const std::wstring& sText );
	void		LogMessageToDB( DBProtocolTable::MsgType mt, CTime time, const std::wstring& sText );

//	CTime		m_timeStartOfDay;
	int			m_nExchangeTimeout;
	int			m_nExchangeNumber;
	int			m_nStatThreshold;
	int			m_nWaitTimeout;

	boost::optional<CTime>	m_timePrevFilm;

	boost::shared_ptr<ExchangeState>	m_pState;

	MessageQueue_t		m_InputQueue;
//	MessageQueue_t		m_OutputQueue;

	std::queue< boost::shared_ptr<OutputMessage> >	 m_OutputQueue;

	SyncCriticalSection	m_WorkingThreadSync;
	SyncCriticalSection	m_ChangeStateSync;

	MyDebugOutputImpl	m_Debug;
	ThreadSync			m_WorkingThread;

	// we process the whole cinema but can get only processed RoomID's list
	int					m_nRoomID;

	//CCinemaOleDB		m_db;
	std::wstring		m_sConnestion;
	LONG				m_State;
	CComPtr<IBoxOfficeRequest>	m_pBoxOffice;
};

#endif // _REQUEST_IMPL_7171562827710921_