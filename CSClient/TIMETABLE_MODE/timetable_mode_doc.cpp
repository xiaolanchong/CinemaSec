#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "timetable_mode_doc.h"
#include "timetable_mode_view.h"

#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTimeTableDoc, CDocument)

BEGIN_MESSAGE_MAP(CTimeTableDoc, CDocument)
  //{{AFX_MSG_MAP(CTimeTableDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CTimeTableDoc::CTimeTableDoc()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CTimeTableDoc::~CTimeTableDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CDiagnosticView diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CTimeTableDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CTimeTableDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CBoxOfficeView class. */
//-------------------------------------------------------------------------------------------------
CTimeTableView * CTimeTableDoc::GetTimeTableView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CTimeTableView ) ))
    {
      return reinterpret_cast<CTimeTableView*>( p );
    }
  }
  return 0;
}

void  CTimeTableDoc::Validate( const TimeTable_t& tt)
{
	TimeHelper th;
	th.Validate(tt);
}

void		CTimeTableDoc::Save(int nRoomID, const CTime& timeBegin, const CTime&  timeEnd, const TimeTable_t& tt, const std::vector<std::wstring>& Names)
{
#if 1
	TimeHelper th;
	th.Validate(tt);
#endif
	try
	{
		CCinemaOleDB& db = dynamic_cast<  CCinemaOleDB& >( GetDB() );
		FilmTime ft;
		ft.m_timeBegin	= timeBegin;
		ft.m_timeEnd	= timeEnd;

		if( !db.CheckConnection())
		try
		{
			db.Open();
		}
		catch(DataBaseException)
		{
			AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
			return;
		}

		db.GetTableTimetableFacet().SetTimetable( nRoomID, ft, tt, Names );
	}
	catch( std::bad_cast )
	{
	}
}

void		CTimeTableDoc::Load(int nRoomID, const CTime& timeBegin, const CTime&  timeEnd, TimeTable_t& tt, std::vector<std::wstring>& Names)
{
	try
	{
		CCinemaOleDB& db = dynamic_cast<  CCinemaOleDB& >( GetDB() );
		FilmTime ft;
		ft.m_timeBegin = timeBegin;
		ft.m_timeEnd	= timeEnd;

		if( !db.CheckConnection())
		try
		{
			db.Open();
		}
		catch(DataBaseException)
		{
			AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
			return;
		}

		db.GetTableTimetableFacet().GetTimetable( nRoomID, ft, tt, Names );
	}
	catch( std::bad_cast ){}
	catch( DataBaseException )
	{
		AfxMessageBox( IDS_ERROR_DB_LOADING, MB_OK|MB_ICONERROR );
	}
}

