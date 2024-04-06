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
#include "stdafx.h"
#include "MessageProcessor.h"
#include "../../CSChair/xml/xmllite.h"
#include "../res/en/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		ELEM_ROOT		_T("component")
#define		ELEM_TIME		_T("time")
#define		ELEM_CINEMA		_T("theatre")
#define		ELEM_ROOM		_T("room")
#define		ELEM_FILM		_T("repertoire")
#define		ELEM_BEGIN		_T("time_begin")
#define		ELEM_END		_T("time_end")
#define		ELEM_NAME		_T("name_film")
#define		ELEM_ERRORDESC	_T("describe")

#define		ELEM_ERROR		_T("error")

#define		ELEM_TIME		_T("time")
#define		ELEM_SEAT		_T("quantity_seat")

#define		TAG_VERSION		_T("version")
#define		TAG_TYPE		_T("type")
#define		TAG_ID			_T("id")
#define		TAG_FILMID		_T("id")
#define		TAG_REFID		_T("ref_id")
#define		TAG_NUMBER		_T("number")

#if 0

//#define		LOCAL_DESC(x)	(x)
std::wstring	Desc_SyntaxError(const std::wstring& s, UINT nID)
{
	CString out; 
	out.Format( LoadStringInternal(IDS_EE_INVALID_TIME_FORMAT), s.c_str() );
	return std::wstring( out );
}

std::wstring	Desc_InvalidType(const std::wstring& s)
{
	return Desc_SyntaxError(s, IDS_EE_INVALID_QUERY_TYPE);
}

std::wstring	Desc_InvalidTime(const std::wstring& s)
{
	return Desc_SyntaxError(s, IDS_EE_INVALID_TIME_FORMAT);
}

std::wstring	Desc_SyntaxError(UINT nID)
{
	CString out; 
	out.LoadString( nID );
	return std::wstring( out );
}

#endif

//======================================================================================//
//                                class MessageProcessor                                //
//======================================================================================//
MessageProcessor::MessageProcessor(IDebugOutput* pDebug):
	m_Debug( pDebug ) 
{
}

MessageProcessor::~MessageProcessor()
{
}

boost::shared_ptr<InputMessage>	
	MessageProcessor::ProcessMessage( const std::wstring& sMsg )
try
{
	XmlLite::XMLDocument doc;
	try
	{
	doc.LoadFromString( sMsg.c_str() );
	}
	catch( XMLException  )
	{
		throw InvalidQueryException( c_DefErrorID )  ;
	}
	XmlLite::XMLElement elRoot = doc.GetRoot();

	int nQueryID = c_DefErrorID;
	try
	{
		elRoot.GetAttr( TAG_ID, nQueryID );
	}
	catch( XMLException ){}
	
	try
	{
		if( elRoot.Name() != ELEM_ROOT )
		{
			std::tstring s = elRoot.Name();
			m_Debug.PrintW( IDebugOutput::mt_error, L"Invalid root tag name, expected %s, provided %s", ELEM_ROOT, s.c_str() );
			throw InvalidQueryException(  nQueryID);
		}
		std::tstring sType, sVersion;
		try
		{
		elRoot.GetAttr( TAG_VERSION, sVersion );
		}
		catch( XMLException )
		{
			throw InvalidQueryException( nQueryID );
		}
		if( sVersion != _T("1.0") )
		{
			throw InvalidVersionException( nQueryID );
		}

		elRoot.GetAttr( TAG_TYPE, sType );
		if(		sType == _T("REPERTOIRES") )
		{
			return ParseTable( elRoot );
		}
		else if( sType == _T("BOERROR") )
		{
			try
			{
				return ParseError( elRoot );
			}
			catch( XMLException )
			{
				m_Debug.PrintW( IDebugOutput::mt_error, L"Syntax error in error message=%s", sMsg.c_str() );
				return boost::shared_ptr<InputMessage>();
			};
		}
		else
		{
			m_Debug.PrintW( IDebugOutput::mt_error, L"Invalid type=%s", sType.c_str() );
			throw InvalidTypeException( nQueryID );
		}
	}
	catch( XMLException )
	{
		throw InvalidQueryException( nQueryID);
	}
}
catch(XMLException)
{
	throw InvalidQueryException(c_DefErrorID);
};

static CTime ConvertTime( const std::wstring& sTime, int nQueryID )
{
	CComVariant var( sTime.c_str() );
	HRESULT hr = var.ChangeType(VT_DATE);
	if( hr != S_OK ) throw InvalidDateTimeException( nQueryID );
	SYSTEMTIME st;
	VariantTimeToSystemTime( var.date, &st );
	if( hr != S_OK ) throw InvalidDateTimeException( nQueryID );
	return CTime (st); 
}

boost::shared_ptr<InputMessage>	MessageProcessor::ParseTable( XmlLite::XMLElement& el )
{
	int nQueryID;
	el.GetAttr( TAG_ID, nQueryID );
	XmlLite::XMLElement::iterator it = el.begin();
	XmlLite::XMLElement elTime( *it );
	if( elTime.Name() != ELEM_TIME ) 
		throw InvalidQueryException( nQueryID);
	std::tstring sTime = elTime.GetValue( );
	CTime timeBegin = ConvertTime( sTime, nQueryID );
	CTime timeEnd = timeBegin + CTimeSpan( 1, 0, 0, 0 );
	BO_TimeTable::FilmMap_t fm;

	++it;
	XmlLite::XMLElement elCinema( *it );
	if( elCinema.Name() != ELEM_CINEMA ) 
		throw InvalidQueryException( nQueryID);
	std::tstring sCinema = elCinema.GetValue();
	XmlLite::XMLElement elEnd(*it);
	++it;
	for( ; it != el.end(); ++it )
	{
		XmlLite::XMLElement elRoom(*it);
		if( elRoom.Name() != ELEM_ROOM ) 
			throw InvalidQueryException( nQueryID);
		int nRoomNo;
		elRoom.GetAttr( TAG_NUMBER, nRoomNo );
		ParseRoom( elRoom,nQueryID, fm[nRoomNo] );
	}

	return boost::shared_ptr<InputMessage> ( new BO_TimeTable( nQueryID, sCinema, timeBegin, timeEnd, fm ) );
}

boost::shared_ptr<InputMessage>	MessageProcessor::ParseError( XmlLite::XMLElement& el )
{
	int nRefID;
	el.GetAttr( TAG_REFID, nRefID );

	BO_ErrorMessage::ErrorArr_t err;
	err.push_back( std::make_pair( 0, std::wstring(L"No error") ) );
	return boost::shared_ptr<InputMessage> ( new BO_ErrorMessage( nRefID, err ) );
}

void	MessageProcessor::ParseRoom(XmlLite::XMLElement& el, int nQueryID,
									std::vector<BO_TimeTable::Item>& Items )
{
	XmlLite::XMLElement::iterator it = el.begin();
	for( ; it != el.end(); ++it )
	{
		XmlLite::XMLElement elFilm(*it);
		if( elFilm.Name() != ELEM_FILM ) 
			throw InvalidQueryException(nQueryID);
		int nFilmID;
		boost::optional<CTime>			Begin, End;
		boost::optional<std::wstring>	sName;
		ParseFilm( elFilm, nQueryID, nFilmID, Begin, End, sName );
		Items.push_back( BO_TimeTable::Item( nFilmID, Begin, End, sName ) );
	}
}

void	MessageProcessor::ParseFilm(	XmlLite::XMLElement& el,
										int	nQueryID,
										int& nFilmID, 
										boost::optional<CTime>&			Begin, 
										boost::optional<CTime>&			End,
										boost::optional<std::wstring>&	sName)
{
	XmlLite::XMLElement::iterator it = el.begin();
	Begin.reset();
	End.reset();
	sName.reset();
	el.GetAttr( TAG_FILMID, nFilmID );
	// non negative FilmID are only allowed
	if( nFilmID <= 0 ) throw InvalidQueryException( nQueryID );
	if( it == el.end() )
	{
		return;
	}
	try
	{
		std::tstring sValue;
		XmlLite::XMLElement elBegin(*it);
		if( elBegin.Name() != ELEM_BEGIN ) throw InvalidQueryException( nQueryID);
		sValue = elBegin.GetValue();
		Begin = ConvertTime( sValue, nQueryID );
		++it;
		XmlLite::XMLElement elEnd(*it);
		if( elEnd.Name() != ELEM_END ) throw InvalidQueryException (nQueryID);
		sValue = elEnd.GetValue();
		End = ConvertTime( sValue, nQueryID );
		++it;
		XmlLite::XMLElement elName(*it);
		if( elName.Name() != ELEM_NAME ) throw InvalidQueryException( nQueryID);
		sName = elName.GetValue();
	}
	catch( XMLException )
	{
		throw WholeInfoException( nQueryID );
	}
}

std::wstring	MessageProcessor::GetTextMessage( const CCS_ErrorMessage& ErrMsg )
{
	XmlLite::XMLDocument doc;
	doc.m_pDoc->put_preserveWhiteSpace( VARIANT_TRUE );
	doc << XmlLite::XMLProcInstr( doc, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"") );
	XmlLite::XMLElement elRoot( doc, ELEM_ROOT ), 
						elError( doc, ELEM_ERROR ),
						elDesc( doc, ELEM_ERRORDESC ) ;
	elRoot	<< XmlLite::XMLAttribute( elRoot, TAG_VERSION, _T("1.0")		)
			<< XmlLite::XMLAttribute( elRoot, TAG_TYPE,	_T("CCSERROR")	)
			<< XmlLite::XMLAttribute( elRoot, TAG_REFID,	ErrMsg.GetID()		);
	elError	<< XmlLite::XMLAttribute( elRoot, TAG_ID, ErrMsg.GetErrorID() );
	elDesc	<< XmlLite::XMLText( elError, ErrMsg.GetDesc()	);
	doc.SetRoot( elRoot );
	elRoot << ( elError << elDesc );
	std::wstring s;
	doc.SaveToString( s );
	return s;
}

std::wstring	MessageProcessor::GetTextMessage( const CCS_ReportMessage& Msg )
{
	XmlLite::XMLDocument doc;
	doc << XmlLite::XMLProcInstr( doc, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"") );
	XmlLite::XMLElement elRoot( doc, ELEM_ROOT ), 
						elTime( doc, ELEM_TIME );
	elRoot	<< XmlLite::XMLAttribute( elRoot, TAG_VERSION, _T("1.0")		)
			<< XmlLite::XMLAttribute( elRoot, TAG_TYPE,	_T("SEAT")	)
			<< XmlLite::XMLAttribute( elRoot, TAG_ID,	Msg.GetID()		);
	CStringW sTime = Msg.GetEndTime().Format( _T("%d.%m.%Y %H:%M:%S") );
	elTime	<< XmlLite::XMLText( elRoot, (LPCWSTR)sTime );
	
	doc.SetRoot( elRoot );
	elRoot << elTime;

	const std::vector< std::pair<int, int> >& StatArr = Msg.GetFilmStat();
	for( size_t i =0; i < StatArr.size(); ++i )
	{
		XmlLite::XMLElement elFilm( elRoot, ELEM_FILM ), 
							elSeat( elRoot, ELEM_SEAT );
		elFilm	<< XmlLite::XMLAttribute( elRoot, TAG_ID,	StatArr[i].first ); 
		elSeat	<< XmlLite::XMLText( elRoot, StatArr[i].second );
		elRoot << (elFilm	<< elSeat);
	}
	std::wstring s;
	doc.SaveToString( s );
	return s;
}