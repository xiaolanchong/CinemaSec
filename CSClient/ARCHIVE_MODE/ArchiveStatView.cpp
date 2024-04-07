// VIDEO_MODE\VideoStatView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "archive_mode_doc.h"
#include "ArchiveStatView.h"

#include "../DBFacet/CfgDB.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../CommonView/wv_message.h"
#include "../gui/Balloon/BalloonHelp.h"

// CArchiveStatView

const UINT	IDT_DBTimer			= 0xff1;
const UINT	Period_DBTimer		= 1000 * 2; //ms

IMPLEMENT_DYNCREATE(CArchiveStatView, CArrangeStatView)

CArchiveStatView::CArchiveStatView():
	CArrangeStatView( IDD_ARCHIVE_STAT )/*,
	DBRoomChooser<CArchiveStatView>( *this )*/
{
//	GetStatisticsDB().Register( this );
}

CArchiveStatView::~CArchiveStatView()
{
//	GetStatisticsDB().Unregister( this );
}


BEGIN_MESSAGE_MAP(CArchiveStatView, CArrangeStatView)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND( IDC_BUTTON_GET_ARCHIVE, OnButtonArchiveRefresh )
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CArchiveStatView drawing

CArchiveDoc* CArchiveStatView::GetDocument()
{
	return dynamic_cast<CArchiveDoc*>( m_pDocument );
}

void CArchiveStatView::OnInitialUpdate()
{
	static bool bNotInit = true;
	CArrangeStatView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 1;
	SetScrollSizes(MM_TEXT, sizeTotal);

	if( bNotInit )
	{
		bNotInit = false;

		m_wndBeginTime.SubclassDlgItem( IDC_TIME_BEGIN, this );
		m_wndEndTime.SubclassDlgItem( IDC_TIME_END, this );
		// NOTE : time format, not localized strings
		m_wndBeginTime.SetFormat( _T("dd.MM.yy HH:mm") );
		m_wndEndTime.SetFormat( _T("dd.MM.yy HH:mm") );
		CTime timeBegin = CTime::GetCurrentTime() - CTimeSpan(1, 0, 0, 0);
		CTime timeEnd = CTime::GetCurrentTime();
		m_wndBeginTime.SetTime( &timeBegin );
		m_wndEndTime.SetTime( &timeEnd );

		m_wndStatistics.ModifyStyle( 0, 
			TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT	);

		m_wndStatistics.ModifyExStyle( 0, TVOXS_ROWSEL|TVOXS_NOSORTHEADER|TVOXS_HGRID|TVOXS_VGRID );
		CString sName, sNumber, sDate, sStartTime, sEndTime;
		sName.LoadString(		IDS_TITLE_NAME		);
		sNumber.LoadString(		IDS_TITLE_NUMBER	);
		sDate.LoadString(		IDS_TITLE_DATE		);
		sStartTime.LoadString(	IDS_TITLE_STARTTIME );
		sEndTime.LoadString(	IDS_TITLE_ENDTIME	);

		LV_COLUMN lvc;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx	= 75;
		lvc.pszText = (LPTSTR)(LPCTSTR)sName;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		m_wndStatistics.SetColumn( 0, &lvc );
		m_wndStatistics.InsertColumn( Col_Number, sNumber, LVCFMT_LEFT, 20 );
		m_wndStatistics.InsertColumn( Col_Date, sDate, LVCFMT_LEFT, 50 );
		m_wndStatistics.InsertColumn( Col_BeginTime, sStartTime, LVCFMT_LEFT, 50 );
		m_wndStatistics.InsertColumn( Col_EndTime, sEndTime, LVCFMT_LEFT, 50 );
		// for human statistics
#ifdef USE_HUMAN_COUNT
		m_wndStatistics.InsertColumn( Col_HumanNumber, _T("Actual"), LVCFMT_LEFT, 50 );
#endif		

		m_LayoutManager.Attach(this);

		m_LayoutManager.SetDefaultConstraint(IDC_TAB_CINEMA);
		m_LayoutManager.SetDefaultConstraint(IDC_TAB_ROOM);
		m_LayoutManager.SetDefaultConstraint(IDC_BUTTON_GET_ARCHIVE);
		m_LayoutManager.SetDefaultConstraint(IDC_LIST_STAT);

		m_LayoutManager.SetConstraint(IDC_TIME_BEGIN, OX_LMS_LEFT, OX_LMT_SAME, 2);
		m_LayoutManager.SetConstraint(IDC_TIME_BEGIN, OX_LMS_RIGHT, OX_LMT_POSITION, 48);

		m_LayoutManager.SetConstraint(IDC_TIME_END, OX_LMS_RIGHT, OX_LMT_SAME, -2 );
		m_LayoutManager.SetConstraint(IDC_TIME_END, OX_LMS_LEFT, OX_LMT_OPPOSITE, 2, IDC_TIME_BEGIN);

		m_LayoutManager.SetConstraint(IDC_BUTTON_GET_ARCHIVE, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_BUTTON_GET_ARCHIVE, OX_LMS_LEFT, OX_LMT_SAME, 2);

		m_LayoutManager.SetConstraint(IDC_STATIC_FROM,	OX_LMS_LEFT, OX_LMT_SAME, 0, IDC_TIME_BEGIN);
		m_LayoutManager.SetConstraint(IDC_STATIC_TO,	OX_LMS_LEFT, OX_LMT_SAME, 0, IDC_TIME_END);

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_LIST_STAT, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_LIST_STAT, OX_LMS_BOTTOM, OX_LMT_SAME, -2);
		m_LayoutManager.RedrawLayout();

		SetTimer( IDT_DBTimer, Period_DBTimer, 0 );
		m_btnArchive.SubclassDlgItem( IDC_BUTTON_GET_ARCHIVE, this );
		m_btnArchive.SetThemeHelper(&m_ThemeHelper);
		m_btnArchive.SetFlat( FALSE );

		m_btnArchive.ModifyStyle( 0, BS_OWNERDRAW );

		m_wndArchiveTooltip.Create( this, TTS_BALLOON );

		Init(IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this);
	}
}

void CArchiveStatView::OnDraw(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

// CArchiveStatView diagnostics

#ifdef _DEBUG
void CArchiveStatView::AssertValid() const
{
	CArrangeStatView::AssertValid();
}

void CArchiveStatView::Dump(CDumpContext& dc) const
{
	CArrangeStatView::Dump(dc);
}
#endif //_DEBUG

// CArchiveStatView message handlers

int CArchiveStatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CArrangeStatView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	return 0;
}

BOOL CArchiveStatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style &=  ~(WS_BORDER);
	cs.dwExStyle &=  ~(WS_EX_CLIENTEDGE);
	return CArrangeStatView::PreCreateWindow(cs);
}

void				CArchiveStatView::OnRoomChange(int nRoomID)	
{
	CArrangeStatView::OnRoomChange(nRoomID);
	try
	{
		m_wndStatistics.DeleteAllItems();
//		UpdateTreeWithCheck();
	}
	catch( std::bad_cast )
	{
	}
	catch(CommonException)
	{
	}
}

struct FindFilmForID
{
	int	m_nFilmID;
	FindFilmForID( int nFilmID ) : m_nFilmID(nFilmID){}
	bool operator () (const FilmTime& ft)
	{
		return ft.m_nFilmID == m_nFilmID;
	}
};

void		CArchiveStatView::UpdateTree( CTime timeBegin, CTime timeEnd)
try
{
	BOOL bTestArchive = SHRegGetBoolUSValue(  _T("Software\\ElVEES\\CinemaSec"), _T("TestArchive"), TRUE, FALSE );
	if( bTestArchive )
	{
	// test archive
		timeBegin = CTime( 2005, 7, 11, 0, 0, 0 );
		timeEnd = CTime( 2005, 7, 13, 0, 0, 0 );
	}
	CArchiveDoc* pDoc = GetDocument();
	pDoc->UpdateArchive( timeBegin, timeEnd );

	CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	std::vector<MeasuredFilm_t> MeasuredMoments; 

	int nRoomID = GetCurRoom();
	StatRowSet_t Stat;
	db.GetTableStatFacet().GetStatForPeriod( timeBegin, timeEnd, nRoomID, Stat );

	StatRowSet_t::const_iterator it = Stat.begin();

	std::vector<std::wstring >	FilmNames;
	TimeTable_t	FilmTimeTable;
	db.GetTableTimetableFacet().GetTimetable(
		nRoomID, 
		FilmTime(-1, timeBegin, timeEnd), 
		FilmTimeTable, FilmNames ) ;

	m_wndStatistics.DeleteAllItems();
	std::vector< HTREEITEM > FilmItems;
	for( size_t i = 0; i < FilmTimeTable.size(); ++i )
	{
		CString sDate		= CArrangeDocument::GetLocalizedDate( FilmTimeTable[i].m_timeBegin );
		CString sTimeBegin	= FilmTimeTable[i].m_timeBegin.Format( _T("%H:%M") );
		CString sTimeEnd	= FilmTimeTable[i].m_timeEnd.Format( _T("%H:%M") );

		HTREEITEM hti = m_wndStatistics.InsertItem ( FilmNames[i].c_str() );
		m_wndStatistics.SetSubItem( hti, Col_Date, OX_SUBITEM_TEXT, sDate );
		m_wndStatistics.SetSubItem( hti, Col_BeginTime, OX_SUBITEM_TEXT, sTimeBegin );
		m_wndStatistics.SetSubItem( hti, Col_EndTime, OX_SUBITEM_TEXT, sTimeEnd );
		m_wndStatistics.SetItemData( hti, FilmTimeTable[i].m_nFilmID );
		FilmItems.push_back( hti );
	}

	MeasuredMoments.resize( FilmTimeTable.size() );

	for( ; it != Stat.end(); ++it)
	{
		for( size_t i = 0 ; i < FilmTimeTable.size() ; ++i )
		{
			if( FilmTimeTable[i].m_timeBegin <= it->m_Time &&
				FilmTimeTable[i].m_timeEnd > it->m_Time )
			{
				HTREEITEM htiParent = FilmItems[i];
				CString sTime	= it->m_Time.Format( _T("%H:%M") );
				HTREEITEM hti = m_wndStatistics.InsertItem ( sTime, htiParent );
				CString sSpect;
				sSpect.Format(_T("%d"), it->m_nSpectators);
				//m_wndStatistics.SetSubItem( hti, 1, OX_SUBITEM_TEXT, _T );
				//m_wndStatistics.SetSubItem( hti, 2, OX_SUBITEM_TEXT, sTime );
				m_wndStatistics.SetSubItem( hti, Col_Number, OX_SUBITEM_TEXT, sSpect );
				m_wndStatistics.SetItemData( hti, it->m_nID );
				MeasuredMoments[i].push_back( std::make_pair( it->m_Time, it->m_nSpectators ) );
			}
		}
	}

	ASSERT( FilmTimeTable.size() == MeasuredMoments.size() );
	
	for( size_t i = 0; i < MeasuredMoments.size(); ++i )
	{
		try
		{
			int nSpecs = pDoc->GetFilmSpectatorNumber( MeasuredMoments[i] );
			CString sSpect;
			sSpect.Format( _T("%d"), nSpecs );
			HTREEITEM hti = FilmItems[i];
			m_wndStatistics.SetSubItem( hti, Col_Number, OX_SUBITEM_TEXT, sSpect );
		}
		catch ( SpectatorException ) 
		{
			// no counts
		}
	}
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
}
catch(std::bad_cast)
{

};

void CArchiveStatView::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == IDT_DBTimer)
	{
#if 0
		UpdateTree();
#endif
	}
	CArrangeStatView::OnTimer(nIDEvent);
}

void	CArchiveStatView::UpdateTreeWithCheck()
{
	CTime timeBegin, timeEnd, timeNow = CTime::GetCurrentTime();
	m_wndBeginTime.GetTime(timeBegin);
	m_wndEndTime.GetTime(timeEnd);

	CRect rc;
	if( m_btnArchive.GetSafeHwnd() == 0 )
	{
	//	UpdateTreeWithNoCheck();
		return;
	}
	m_btnArchive.GetWindowRect(&rc);
	UINT nStyle = 	(	CBalloonHelp::unCLOSE_ON_ANYTHING & ~  CBalloonHelp::unCLOSE_ON_MOUSE_MOVE)|
					/*	CBalloonHelp::unSHOW_INNER_SHADOW |*/
						CBalloonHelp::unSHOW_CLOSE_BUTTON
					;
	m_wndStatistics.DeleteAllItems();
	CString sTitle, sContent;
	sTitle.LoadString( IDS_ERROR_INVALIDTIME );
	if( timeBegin > timeEnd )
	{
		sContent.LoadString( IDS_ERROR_DESC_1 );
		CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
		return;
	}
	else if( timeBegin > timeNow )
	{
		sContent.LoadString( IDS_ERROR_DESC_2 );
		CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
		return;
	}
	else if( timeEnd > timeNow )
	{
		sContent.LoadString( IDS_ERROR_DESC_3 );
		CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_WARNING, nStyle );	
		timeEnd = timeNow;
		m_wndEndTime.SetTime(&timeEnd);	
	}
	
	UpdateTree(timeBegin, timeEnd);
}

void CArchiveStatView::OnButtonArchiveRefresh( )
{
	UpdateTreeWithCheck();
}

void	CArchiveStatView::UpdateTreeWithNoCheck()
{
}

void CArchiveStatView::OnDblClickItem(HTREEITEM hti, CPoint pt)
{
	if( m_wndStatistics.GetParentItem(hti) !=
		m_wndStatistics.GetRootItem() )
	{
		int nStatID = m_wndStatistics.GetItemData(hti);
		CArrangeDocument* pDoc = GetDocument();
		pDoc->LoadImageRoomState (nStatID);
	}
	else
	{
		// OXTreeCtrl::OnlButtonDblkClick does the same action, so we invert it
		UINT nFlags;
		int nIndex = m_wndStatistics.HitTest( pt, &nFlags );
		if( (nFlags & TVHT_ONITEM) && !(nFlags & TVHT_ONITEMBUTTON)  && nIndex != -1 )
		{
#ifdef USE_HUMAN_COUNT
			HTREEITEM hti = m_wndStatistics.GetItemFromIndex( nIndex );
			int nLeft = 0;
			for( int i = Col_Name; i < Col_HumanNumber; ++i )
			{
				nLeft += m_wndStatistics.GetColumnWidth(i);
			}
			int nRight = nLeft + m_wndStatistics.GetColumnWidth(Col_HumanNumber);
			CPoint ptActual(pt);
			ptActual.Offset( m_wndStatistics.GetScrollPos( SB_HORZ ), m_wndStatistics.GetScrollPos( SB_VERT ) );
			if( ptActual.x > nLeft && ptActual.x < nRight )
			{
				CRect rcItem;
				m_wndStatistics.GetItemRect( hti, &rcItem, FALSE );
				rcItem.left	= nLeft;
				rcItem.right = nRight;

				ClientToScreen(&rcItem);
				CWnd* wnd = new CWnd;
				BOOL res = wnd->CreateEx( WS_EX_TOPMOST, L"Edit", L"blah", WS_BORDER|WS_VISIBLE, rcItem, 0, 0 );
				ASSERT(res);
			}
			else
#endif
			{
				int nFilmID = m_wndStatistics.GetItemData( hti );
				CArchiveDoc* pDoc = GetDocument();
				pDoc->SeekArchive(nFilmID);
			}
			m_wndStatistics.Expand( hti, TVE_TOGGLE );
		}
	}
}

void CArchiveStatView::OnRClickItem(HTREEITEM hti)
{
	if( m_wndStatistics.GetParentItem(hti) !=
		m_wndStatistics.GetRootItem() )
	{
//		int nID = m_wndStatistics.GetItemData(hti);
	}
	else
	{
		
	}	
}

void	CArchiveStatView::Clear()
{
	m_wndStatistics.DeleteAllItems();
}