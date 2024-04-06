//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	XML parser for input/output messages
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 01.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _MESSAGE_PROCESSOR_8844623526142445_
#define _MESSAGE_PROCESSOR_8844623526142445_

#include "../../CSChair/Common/Exception.h"
#include "../../CSChair/debug_int.h"
#include "InputMessage.h"
#include "TimeTableMessage.h"
#include "QueryException.h"
#include <boost/shared_ptr.hpp>

/*
QUERY_EXCEPTION( MessageProcessorException,	QueryException )
QUERY_EXCEPTION( SyntaxErrorException,		MessageProcessorException )
QUERY_EXCEPTION( InvalidVersionException,	MessageProcessorException )
QUERY_EXCEPTION( InvalidQueryTypeException,	MessageProcessorException )
QUERY_EXCEPTION( InvalidTimeException,		MessageProcessorException )*/

//======================================================================================//
//                                class MessageProcessor                                //
//======================================================================================//

namespace XmlLite
{
	class XMLElement;
}

class MessageProcessor
{
	boost::shared_ptr<InputMessage>	ParseTable( XmlLite::XMLElement& el );
	boost::shared_ptr<InputMessage>	ParseError( XmlLite::XMLElement& el );
	
	void	ParseRoom(  XmlLite::XMLElement& el, int nQueryID, std::vector<BO_TimeTable::Item>& Items );
	void	ParseFilm(  XmlLite::XMLElement& el,
						int nQueryID,
						int& nFilmID, 
						boost::optional<CTime>&			Begin, 
						boost::optional<CTime>&			End,
						boost::optional<std::wstring>&	sName);

	MyDebugOutputImpl	m_Debug;
public:
	MessageProcessor(IDebugOutput* pDebug);
	virtual ~MessageProcessor();
	
	// throws MessageProcessorException
	boost::shared_ptr<InputMessage>	ProcessMessage( const std::wstring& sMsg );

	std::wstring					GetTextMessage( const CCS_ErrorMessage& msg ); 
	std::wstring					GetTextMessage( const CCS_ReportMessage& msg ); 
};

#endif // _MESSAGE_PROCESSOR_8844623526142445_