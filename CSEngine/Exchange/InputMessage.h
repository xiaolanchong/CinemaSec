//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Input message for CinemaSec exchange component from BO
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 25.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _INPUT_MESSAGE_8731254542543321_
#define _INPUT_MESSAGE_8731254542543321_

#include <boost/optional.hpp>
#include <map>
#include "..\..\CSChair\debug_int.h"
#include "QueryException.h"
#include "..\DBFacet\DBRows.h"

/*
QUERY_EXCEPTION(  TimeTableMessageException,	QueryException				)
QUERY_EXCEPTION(  InvalidTimeTableException,	TimeTableMessageException	)
QUERY_EXCEPTION(  InvalidFilmIDException,		TimeTableMessageException	)*/
//======================================================================================//
//                                  class InputMessage                                  //
//======================================================================================//

class CCinemaOleDB;

class OutputMessage
{
	int				m_nMsgID;	// id or refid
public:
	int						GetID()		const { return m_nMsgID;	}
	virtual void			Dump(MyDebugOutputImpl& Debug) const = 0;
	virtual std::wstring	GetText() const = 0;
	virtual DBProtocolTable::MsgType			GetDbType() const = 0;

	OutputMessage( int nID ): m_nMsgID ( nID ){} 
	virtual ~OutputMessage() {};
};

class InputMessage
{
public:
	virtual void								Dump(MyDebugOutputImpl& Debug) const = 0;
	virtual void								Validate() const = 0;
	virtual boost::shared_ptr<OutputMessage>	Execute( CCinemaOleDB& db, MyDebugOutputImpl& Debug ) = 0;

	virtual DBProtocolTable::MsgType			GetDbType() const = 0;

	virtual ~InputMessage();
};

class BO_ErrorMessage : public InputMessage
{
public:
	typedef std::vector< std::pair<int, std::wstring> >	ErrorArr_t;
private:
	ErrorArr_t	m_Errors;
	int			m_nRefID;
public:
	BO_ErrorMessage( int nRefID, const ErrorArr_t& err ):  
	  m_Errors(err),
	  m_nRefID(nRefID)
	  {}
	const ErrorArr_t&	GetErrors() const { return m_Errors;}
	int					GetRefID() const { return m_nRefID; }
	virtual void		Dump(MyDebugOutputImpl& Debug) const ;
	virtual void										Validate() const {};
	virtual boost::shared_ptr<OutputMessage>			Execute( CCinemaOleDB& db, MyDebugOutputImpl& Debug ) 
	{
		return boost::shared_ptr<OutputMessage>();
	};

	virtual DBProtocolTable::MsgType			GetDbType() const { return DBProtocolTable::BO_Response;}
};

class CCS_ErrorMessage : public OutputMessage
{
	int				m_nErrorID;
	std::wstring	m_sDesc;
public:
	CCS_ErrorMessage( int nMsgID, int nErrorID, const std::wstring& sDesc ) : 
		OutputMessage(nMsgID), 
		m_nErrorID(nErrorID), 
		m_sDesc(sDesc)
	{};

	std::wstring			GetDesc()	const	{ return m_sDesc;		}
	int						GetErrorID()const	{ return m_nErrorID;	}

	virtual void			Dump(MyDebugOutputImpl& Debug) const;
	virtual std::wstring	GetText() const;
	virtual DBProtocolTable::MsgType			GetDbType() const { return DBProtocolTable::CCS_Response;}
};

class CCS_ReportMessage : public OutputMessage
{
	CTime								m_timeEnd;
	std::vector<std::pair<int, int>	>	m_Statistics;
public:
	CCS_ReportMessage( int nID, CTime timeEnd, const std::vector<std::pair<int, int> >& Statistics ) : 
	  OutputMessage(nID), 
		  m_timeEnd(timeEnd), 
		  m_Statistics(Statistics)
	  {};

	  const std::vector<std::pair<int, int> >		GetFilmStat() const	{ return m_Statistics; }
	  CTime											GetEndTime() const { return m_timeEnd; }

	  virtual void			Dump(MyDebugOutputImpl& Debug) const;
	  virtual std::wstring	GetText() const;

	  virtual DBProtocolTable::MsgType			GetDbType() const { return DBProtocolTable::CCS_Statistics;}
};

#endif // _INPUT_MESSAGE_8731254542543321_