// VideoDocument.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "ReportDocument.h"
#include "ReportView.h"
#include "ReportParamView.h"
#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../../CSChair/XML/XmlLite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportDocument

IMPLEMENT_DYNCREATE(CReportDocument, CDocument)

CReportDocument::CReportDocument()
{
}

CReportDocument::~CReportDocument()
{
}


BEGIN_MESSAGE_MAP(CReportDocument, CDocument)
	//{{AFX_MSG_MAP(CReportDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportDocument diagnostics

#ifdef _DEBUG
void CReportDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CReportDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportDocument commands

void	CReportDocument::FillRoomTree( CTreeCtrl* pTree, int nCinemaID  )
try
{
	CCinemaDB& db = GetDB();

	pTree->DeleteAllItems();
	std::set<int>	Rooms = db.GetTableRoomFacet().SelectRoomsFromCinema( nCinemaID );
	std::set<int>::const_iterator it = Rooms.begin();
	for(; it != Rooms.end(); ++it)
	{
		CString s;
		int nRoomID = *it;
		std::set<RoomRow>::const_iterator it = GetDB().GetTableRoomFacet().GetRooms().find( nRoomID );
		const RoomRow& Room = *it;

		s.Format( _T("%d"), Room.m_nNumber );
		HTREEITEM hti = pTree->InsertItem( s );
		pTree->SetCheck( hti, TRUE );
		pTree->SetItemData( hti, nRoomID );
	}
}
catch( DataBaseException )
{
	pTree->DeleteAllItems();
}
catch(  std::bad_cast )
{
	pTree->DeleteAllItems();
};

bool	CReportDocument::CreateFirstReport( CTime timeBegin, CTime timeEnd, const std::vector<int>& SelectedRooms)
try
{
	CCinemaOleDB& db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	StatRowSet_t StatRows;
	db.GetTableStatFacet().GetStatForPeriodReport( timeBegin, timeEnd,SelectedRooms, StatRows );
	XmlLite::XMLDocument xmlDoc ;
	StatRowSet_t::const_iterator it = StatRows.begin();

	XmlLite::XMLElement elRoot( xmlDoc, _T("report") );
	for(; it !=  StatRows.end(); ++it )
	{
		// NOTE if you changed delimiter "-"  , change report1.xsl too!!!
		// FIXME : localized string requires
		CString sTime = it->m_Time.Format( "%d.%m.%y-%H:%M" );
		XmlLite::XMLElement elRec ( elRoot, _T("record") );
		elRec	<< XmlLite::XMLAttribute(elRoot, _T("date"), sTime )
				<< XmlLite::XMLAttribute(elRoot, _T("room"), it->m_nRoomID )
				<< XmlLite::XMLAttribute(elRoot, _T("total"), 0 )
				<< XmlLite::XMLAttribute(elRoot, _T("measured"), it->m_nSpectators );
		elRoot	<< elRec;
	}
	xmlDoc.SetRoot( elRoot );
	GetView <CReportView> ()->CreateReport( xmlDoc );
	return true;
}
catch( XMLException )
{
	return false;
}
catch( DataBaseException )
{
	CCinemaOleDB*	db = dynamic_cast< CCinemaOleDB* >( &GetDB() );
	if( db && !db->CheckConnection())
	{
		try
		{
			db->Open();
		}
		catch(DataBaseException)
		{
			AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
		}
	}
	return false;
}
catch ( std::bad_cast ) 
{
	return false;
};

void	CReportDocument::PreviewReport()
{
	GetView <CReportView> ()->PreviewReport(  );
}

void	CReportDocument::PrintReport()
{
	GetView <CReportView> ()->PrintReport(  );
}

void	CReportDocument::SaveReport()
{
	GetView <CReportView> ()->SaveReport(  );
}