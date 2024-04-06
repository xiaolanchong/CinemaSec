#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "boxoffice_mode_doc.h"
#include "boxoffice_mode_view.h"
#include "ExchangeReport.h"

#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBoxOfficeDoc, CDocument)

BEGIN_MESSAGE_MAP(CBoxOfficeDoc, CDocument)
  //{{AFX_MSG_MAP(CBoxOfficeDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeDoc::CBoxOfficeDoc()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeDoc::~CBoxOfficeDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CBoxOfficeDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CBoxOfficeDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CBoxOfficeDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CBoxOfficeView class. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeView * CBoxOfficeDoc::GetBoxOfficeView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CBoxOfficeView ) ))
    {
      return reinterpret_cast<CBoxOfficeView*>( p );
    }
  }
  return 0;
}

CString		CBoxOfficeDoc::CreateReport( int nCinemaID, CTime timeFrom, CTime timeTo )
try
{
	CCinemaOleDB& db = dynamic_cast< CCinemaOleDB& >( GetDB() );

	std::auto_ptr< ExchangeReport > rep( new ExchangeReport );

	std::vector< DBProtocolTable::Message_t > Msgs;
	db.GetProtocolTable().GetMessage( nCinemaID, timeFrom, timeTo, Msgs );

	for( size_t i = 0; i < Msgs.size(); ++i )
	{
		rep->AddMessage( Msgs[i] );
	}
/*
	rep->AddMessage( DBProtocolTable::Message_t( DBProtocolTable::BO_Timetable, timeFrom, std::wstring(L"Время") ) );
	rep->AddMessage( DBProtocolTable::Message_t( DBProtocolTable::BO_Response, timeFrom, std::wstring(L"Текст") ) );
	rep->AddMessage( DBProtocolTable::Message_t( DBProtocolTable::CCS_Statistics, timeFrom, std::wstring(L"Тип") ) );
	rep->AddMessage( DBProtocolTable::Message_t( DBProtocolTable::CCS_Response, timeFrom, std::wstring(L"blah-blah") ) );
	rep->AddMessage( DBProtocolTable::Message_t( DBProtocolTable::BO_Unknown, timeFrom, std::wstring(L"blah-blah") ) );
*/
	CString sFileName = rep->GetFileName();
	return sFileName;
}
catch(DataBaseException)
{
	AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
	return CString();
}
catch( std::bad_cast )
{
	return CString();
};




